///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// ������ ��� �������� ������-����� ������ ���������.
// 
// ���� � �������� ����� ������.
// 
// ������: 1.0.0.2
// ���� ���������� ���������: 18:11 28.01.2023
// 
// �����: ������ �.�. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "Logs.h"
#include "logs_config_private.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#undef LogLine
#undef LogFLine
#undef LogAddTrace
#undef LogBeginDump

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief ��������� ����� �����.
*/
struct LogFile
{
    /// ��������� �� ����.
    FILE* file;
    /// ��������� �����.
    LogSignature sig;
    /// ���������.
    const char* caption;
    /// ��� �����.
    const char* fileName;
};

/// ��������� � ������ �����������.
static const char* const LogLevelMessages[] =
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// ������ ������ ��� �������������� ���� � ������� ��������� �����.
static const size_t BufferTimeSize  = 40;

/// ���������� ���-������.
static const size_t LogFilesSize    = (size_t)LogSignature::Reserved;
/// ������ �������� ������������.
static LogFile      LogFiles[LogFilesSize];
/// ��� �� ������ ����������� �����.
static bool         LogFilesCreated = false;

/// � ����� ����� ����� ��������� ����������� html-����. ��������� � ���� ������� �� � �����, � ����� ����� ������.
static int          TextOffset      = 0;

/// ����� ���������� � ���� ����� ������.
/// ����� ��� ���� ������.
/// ��� ����� ��������� ������� ��������� � ������ ������.
static size_t       AbsoluteLogLineIndex = 0;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  ����������� ���������� ����� �����. ���������� ������ LogsConstructor().
 * 
 * @param logFileName ��� ��������� ����� ����� (������� ���� � ����).
 * @param caption     ��������� ��������� � ������ ����� �����.
 * @param sig         ��������� ����� �����.
 * 
 * @return LogError::FileOpen, ���� �� ������� ������� ����. 
 *         LogError::NoErrors, ���� ��� ������.
*/
static LogError LogConstructor(const char* const logFileName, const char* const caption, const LogSignature sig);

/**
 * @brief  ���������� ��������� �� ���� ������� ������ �����������.
 * 
 * @param level ������� �����������.
 * 
 * @return ��������� �� ���� ������.
*/
static const char* const GetLogLevelColor(const LogLevel level);

/**
 * @brief  ������� � ����� ����������������� ������� �����.
 * 
 * @param buffer �������� ����������������� �����. ������ ������ ������ ���� �� ������ BufferTimeSize.
*/
static void FormatCurrentTime(char* const buffer);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

LogError LogsConstructor()
{
    LogFilesCreated = true;
    
    system("rd " LOGS_FOLDER "/s /q");
    system("md " LOGS_FOLDER);

    LogError status = LogError::NoErrors;

    status = LogConstructor(LOGS_FOLDER "log_general.html", "����� ��� ���������.", LogSignature::General);
    if (status != LogError::NoErrors)
    {
        return status;
    }

    ///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
    // �������� ��� ���� ���� ����� �����. � ������ ������ �� ������ ������� ��� �������� ����� � ������.
    ///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

    // � ����������� ���� �������� �� ��������� �����.
    atexit(LogsDestructor);

    return status;
}

static LogError LogConstructor(const char* const logFileName, const char* const caption, const LogSignature sig)
{
    assert(logFileName);
    assert(caption);

    const size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    FILE* const file = fopen(logFileName, "w");
    if (!file)
    {
        return LogError::FileOpen;
    }

    // ������ ����� ����������������.
    setbuf(file, nullptr);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    LogFiles[logIndex].file     = file;
    LogFiles[logIndex].fileName = logFileName;
    LogFiles[logIndex].caption  = caption;
    LogFiles[logIndex].sig      = sig;

    char buffer[BufferSize]         = "";
    char bufferTime[BufferTimeSize] = "";
    FormatCurrentTime(bufferTime);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    snprintf(buffer, BufferSize, 
             "<html>\n"
             "<head>"
             "<meta charset=\"" LOG_CODEPAGE "\">"
             "<title>%s</title>"
             "<style>font {line-height: " LOG_LINE_HEIGHT ";} "
             "body {background-color: " LOG_BACKGROUND_COLOR ";  font-size: " LOG_TEXT_SIZE "} "
             "head {background-color: " LOG_BACKGROUND_COLOR ";}"
             "</style>"
             "</head>\n"
             "<body>\n"
             "<h1><font color=\"" LOG_HEADER_COLOR "\">%s %s.</font></h1>\n", caption, caption, bufferTime);
    
    fputs(buffer, file);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    TextOffset = ftell(file);

    fputs("</body>\n"
          "</html>\n", file);

    TextOffset -= ftell(file);
    
    return LogError::NoErrors;
}

void LogsDestructor()
{
    assert(LogFilesCreated);

    static bool LogFilesClosed = false;
    if (LogFilesClosed)
        return;

    for (size_t st = 0; st < LogFilesSize; st++)
    {
        LogFile logFile = LogFiles[st];
        if (logFile.file)
            fclose(logFile.file);
    }

    LogFilesClosed = true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

void LogLine(const char* const message, const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* const funcName, const char* const fileName, const int logLine)
{
    assert(message);
    assert(funcName);
    assert(fileName);

    assert(LogFilesCreated);

    const size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    char  buffer[BufferSize]         = "";
    char  bufferTime[BufferTimeSize] = "";
    FormatCurrentTime(bufferTime);

    const char* const logLevelColor  = GetLogLevelColor(logLevel);
    
    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    fseek(LogFiles[logIndex].file, TextOffset, SEEK_END);

    snprintf(buffer, BufferSize, "<pre>"
            "<font color=\"%s\">%20zd> %-.8s [%-.8s] > </font>"
             "<font color = \"" LOG_FUNCT_COLOR "\">%s</font>\n"
             "<font color = \"" LOG_FILE_COLOR "\"> in %s</font>\n"
             "<font color = \"" LOG_LINE_COLOR "\"> at %d line</font>\n"
             "<font color = \"" LOG_MESSAGE_COLOR "\">\t %s</font>"
            "</pre>\n",
            logLevelColor, AbsoluteLogLineIndex++, bufferTime, LogLevelMessages[(size_t)logLevel],
            funcName,
            fileName,
            logLine,
            message);

    fputs(buffer, LogFiles[logIndex].file);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    if (dublicateToConsole)
    {
        fputs(message, stderr);
        fputc('\n', stderr);
    }

    fputs("</body>\n"
          "</html>\n", LogFiles[logIndex].file);
}

void LogFLine(const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* const funcName, const char* const fileName, const int logLine, const char* const format, ...)
{
    assert(format);
    assert(funcName);
    assert(fileName);

    assert(LogFilesCreated);

    const size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    char  buffer[BufferSize]         = "";
    char  bufferTime[BufferTimeSize] = "";
    FormatCurrentTime(bufferTime);

    const char* const logLevelColor  = GetLogLevelColor(logLevel);
    
    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    fseek(LogFiles[logIndex].file, TextOffset, SEEK_END);

    snprintf(buffer, BufferSize, "<pre>"
            "<font color=\"%s\">%20zd %-.8s [%-.8s] > </font>"
            "<font color = \"" LOG_FUNCT_COLOR "\">%s</font>\n"
            "<font color = \"" LOG_FILE_COLOR "\"> in %s</font>\n"
            "<font color = \"" LOG_LINE_COLOR "\"> at %d line</font>\n"
            "<font color = \"" LOG_MESSAGE_COLOR "\">",
            logLevelColor, AbsoluteLogLineIndex++, bufferTime, LogLevelMessages[(size_t)logLevel],
            funcName,
            fileName,
            logLine);
    
    fputs(buffer, LogFiles[logIndex].file);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    va_list args;
	va_start(args, format);
    vsnprintf(buffer, BufferSize, format, args);
    va_end(args);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    if (dublicateToConsole)
    {
        fputs(buffer, stderr);
        fputc('\n', stderr);
    }
    
    fputs(buffer, LogFiles[logIndex].file);
    
    fputs("</font></pre>\n"
          "</body>\n"
          "</html>\n", LogFiles[logIndex].file);
}

void LogAddTrace(const LogSignature sig, const LogTrace trace,
                const char* const funcName, const char* const fileName, const int logLine)
{
    assert(funcName);
    assert(fileName);
    assert(LogFilesCreated);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
        
    const size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    char  buffer[BufferSize]         = "";
    char  bufferTime[BufferTimeSize] = "";
    FormatCurrentTime(bufferTime);

    const char* logColor             = LOG_TRACE_COLOR;
    if (trace == LogTrace::Error)
        logColor = LOG_ERR_TRACE_COLOR;
    
    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    fseek(LogFiles[logIndex].file, TextOffset, SEEK_END);

    snprintf(buffer, BufferSize, 
             "<pre>\n"
                "\t<font color=\"%s\">\n"
             "%20zd %-.8s [%-.8s%d] > %s\n"
             "\t\tin %s\n"
             "\t\tat %d line\n"
                "\t</font>\n"
             "</pre>\n",
             logColor, AbsoluteLogLineIndex++, bufferTime, LogLevelMessages[(size_t)LogLevel::Trace], (int)trace,
             funcName,
             fileName,
             logLine);

    fputs(buffer, LogFiles[logIndex].file);

    fputs("</body>\n"
          "</html>\n", LogFiles[logIndex].file);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

FILE* LogBeginDump(const LogSignature sig, const LogLevel logLevel,
             const char* const funcName, const char* const fileName, const int logLine)
{
    assert(funcName);
    assert(fileName);

    assert(LogFilesCreated);

    const size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    char  buffer[BufferSize]         = "";
    char  bufferTime[BufferTimeSize] = "";
    FormatCurrentTime(bufferTime);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    fseek(LogFiles[logIndex].file, TextOffset, SEEK_END);

    snprintf(buffer, BufferSize,
             "<pre>\n"
             "\t<font color=\"" LOG_DUMP_COLOR "\">\n"
             "%20zd %-.8s [%-.8s] > %s\n"
             "\t\tin %s\n"
             "\t\tat %d line\n",
             AbsoluteLogLineIndex++, bufferTime, LogLevelMessages[(size_t)logLevel],
             funcName,
             fileName,
             logLine);

    fputs(buffer, LogFiles[logIndex].file);

    return LogFiles[logIndex].file;
}

void LogEndDump(const LogSignature sig)
{
    const size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    fputs("\t</font>\n"
          "</pre>\n"
          "</body>\n"
          "</html>\n", LogFiles[logIndex].file);
}

void LogAddImage(const LogSignature sig, const char* imagePath)
{
    assert(imagePath);

    const size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    fseek(LogFiles[logIndex].file, TextOffset, SEEK_END);

    fprintf(LogFiles[logIndex].file, "<img src = \"%s\" height = 600>\n", imagePath);

    fputs("</body>\n"
          "</html>\n", LogFiles[logIndex].file);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static const char* const GetLogLevelColor(const LogLevel level)
{
    switch (level)
    {
        case LogLevel::Debug:
            return LOG_DEBUG_COLOR;
        case LogLevel::Warning:
            return LOG_WARNING_COLOR;
        case LogLevel::Error:
            return LOG_ERROR_COLOR;
        case LogLevel::Info:
            return LOG_INFO_COLOR;
        default:
            assert(!"Not implemented");
            // ������������� ���� ������, ����� ��������� � ����� � Release, ���� assert �� ���������� � Debug.
            return LOG_ERROR_COLOR;
    }
}

static void FormatCurrentTime(char* const buffer)
{
    const time_t    rawTime = time(nullptr);
    const tm* const curTime = localtime(&rawTime);
    strftime(buffer, BufferTimeSize, "%H:%M:%S", curTime);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///