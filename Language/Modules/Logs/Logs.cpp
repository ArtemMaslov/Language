#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <assert.h>
#include <stdarg.h>

#include "Logs.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#undef LogLine
#undef LogFLine
#undef LogTrace
#undef LogBeginDump
#undef LogEndDump

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

/**
 * @brief Структура файла логов.
*/
struct LogFile
{
    /// Указатель на файл.
    FILE* file;
    /// Сигнатура файла.
    LogSignature sig;
    /// Заголовок.
    const char* caption;
    /// Имя файла.
    const char* fileName;
};

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

/// Сообщения о уровне логгирования.
const char* LogLevelMessages[] =
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
};

const size_t bufferSize      = 1024;
const size_t bufferTimeSize  = 40;

const size_t LogFilesSize    = 10;
LogFile      LogFiles[LogFilesSize];
bool         LogFilesCreated = false;
bool         LogFilesClosed  = false;

int          TextOffset      = 0;

/// Номер записанной в файл логов строки.
/// Общий для всех файлов.
/// Так можно отследить порядок сообщений в разных файлах.
size_t       AbsoluteLogLineIndex = 0;

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static int LogConstructor(const char* logFileName, const char* caption, const LogSignature sig);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

int LogsConstructor()
{
    LogFilesCreated = true;
    
    system("rd " LOGS_FOLDER "/s /q");
    system("md " LOGS_FOLDER);

    int status = LOG_NO_ERRORS;

    status |= LogConstructor(LOGS_FOLDER "log_general.html",    "Общий лог программы.",           LOG_SIG_GENERAL);

    status |= LogConstructor(LOGS_FOLDER "log_hash_table.html", "Лог работы хеш-таблицы.",        LOG_SIG_HASH_TABLE);

    status |= LogConstructor(LOGS_FOLDER "log_list.html",       "Лог работы двусвязного списка.", LOG_SIG_LIST);

    atexit(LogsDestructor);

    return status;
}

static int LogConstructor(const char* logFileName, const char* caption, const LogSignature sig)
{
    assert(logFileName);
    assert(caption);
    assert(LogFilesCreated);

    size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    FILE* file = fopen(logFileName, "w");
    
    if (!file)
    {
        LogFLine(LOG_LVL_ERROR, LOG_SIG_GENERAL, true, __FUNCSIG__, __FILE__, __LINE__,
                 "Ошибка открытия файла с логами. FileName = \"%s\"", logFileName);

        return LOG_ERR_FILE_OPENING;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    LogFiles[logIndex].file     = file;
    LogFiles[logIndex].fileName = logFileName;
    LogFiles[logIndex].caption  = caption;
    LogFiles[logIndex].sig      = sig;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
            
    char  bufferTime[bufferTimeSize] = "";
    char  buffer[bufferSize] = "";

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    time_t rawTime = time(nullptr);
    tm*    curTime = localtime(&rawTime);
    strftime(bufferTime, bufferTimeSize, "%H:%M:%S %d.%m.%Y", curTime);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    snprintf(buffer, bufferSize, 
             "<html>\n"
             "<head>"
             "<meta charset=\"CP1251\">"
             "<title>%s</title>"
             "<style>font {line-height: 1.2;} "
             "body {background-color: #303030;  font-size: 12} "
             "head {background-color: #303030;}"
             "</style>"
             "</head>\n"
             "<body>\n"
             "<h1><font color=\"99B333\">%s %s.</font></h1>\n", caption, caption, bufferTime);
    
    fputs(buffer, file);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    TextOffset = ftell(file);

    fputs("</body>\n"
          "</html>\n", file);

    TextOffset -= ftell(file);

    fflush(file);
    
    return LOG_NO_ERRORS;
}

void LogsDestructor()
{
    assert(LogFilesCreated);

    if (LogFilesClosed)
        return;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    for (size_t st = 0; st < LogFilesSize; st++)
    {
        LogFile logFile = LogFiles[st];

        if (logFile.file)
            fclose(logFile.file);
    }

    LogFilesClosed = true;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

void LogLine(const char* message, const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* funcName, const char* fileName, const int logLine)
{
    assert(message);
    assert(funcName);
    assert(fileName);

    assert(LogFilesCreated);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    int status = LOG_NO_ERRORS;

    char  bufferTime[bufferTimeSize] = "";
    char  buffer[bufferSize]         = "";

    const char* logColor             = nullptr;
    
    size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    switch (logLevel)
    {
        case LOG_LVL_DEBUG:
            logColor = "FFA000";
            break;
        case LOG_LVL_WARNING:
            logColor = "FF4D00";
            break;
        case LOG_LVL_ERROR:
            logColor = "D53032";
            break;
        default:
            logColor = "EAE6CA";
            break;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    fseek(LogFiles[logIndex].file, TextOffset, SEEK_END);

    time_t rawTime = time(nullptr);
    tm*    curTime = localtime(&rawTime);
    strftime(bufferTime, bufferTimeSize, "%H:%M:%S", curTime);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    snprintf(buffer, bufferSize, "<pre>"
            "<font color=\"%s\">%20zd> %-.8s [%-.8s] > </font>"
            "<font color = \"D5DED9\">%s</font>\n"
            "<font color = \"B0B7C6\"> in %s</font>\n"
            "<font color = \"FFD700\"> at %d line</font>\n"
            "<font color = \"ADDFAD\">\t %s</font>"
            "</pre>\n",
            logColor, AbsoluteLogLineIndex++, bufferTime, LogLevelMessages[(int)logLevel],
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

    fflush(LogFiles[logIndex].file);
}

void LogFLine(const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* funcName, const char* fileName, const int logLine, const char* format, ...)
{
    assert(format);
    assert(funcName);
    assert(fileName);

    assert(LogFilesCreated);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    char  bufferTime[bufferTimeSize] = "";
    char  buffer[bufferSize] = "";

    const char* logColor     = nullptr;
    
    size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    switch (logLevel)
    {
        case LOG_LVL_DEBUG:
            logColor = "FFA000";
            break;
        case LOG_LVL_WARNING:
            logColor = "FF4D00";
            break;
        case LOG_LVL_ERROR:
            logColor = "D53032";
            break;
        default:
            logColor = "EAE6CA";
            break;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    fseek(LogFiles[logIndex].file, TextOffset, SEEK_END);

    time_t rawTime = time(nullptr);
    tm*    curTime = localtime(&rawTime);
    strftime(bufferTime, bufferTimeSize, "%H:%M:%S", curTime);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    snprintf(buffer, bufferSize, "<pre>"
            "<font color=\"%s\">%20zd %-.8s [%-.8s] > </font>"
            "<font color = \"BDDA57\">%s</font>\n"
            "<font color = \"B0B7C6\"> in %s</font>\n"
            "<font color = \"FFD700\"> at %d line</font>\n"
            "<font color = \"ADDFAD\">",
            logColor, AbsoluteLogLineIndex++, bufferTime, LogLevelMessages[(int)logLevel],
            funcName,
            fileName,
            logLine);
    
    fputs(buffer, LogFiles[logIndex].file);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    va_list args;
	va_start(args, format);
    vsnprintf(buffer, bufferSize, format, args);
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

    fflush(LogFiles[logIndex].file);
}

void LogTrace(const LogSignature sig,
                const char* funcName, const char* fileName, const int logLine)
{
    assert(funcName);
    assert(fileName);
    assert(LogFilesCreated);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    int status = LOG_NO_ERRORS;

    char  bufferTime[bufferTimeSize] = "";
    char  buffer[bufferSize]         = "";

    const char* logColor             = "F8CA00";

    size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    fseek(LogFiles[logIndex].file, TextOffset, SEEK_END);

    time_t rawTime = time(nullptr);
    tm*    curTime = localtime(&rawTime);
    strftime(bufferTime, bufferTimeSize, "%H:%M:%S", curTime);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    snprintf(buffer, bufferSize, 
             "<pre>\n"
                "\t<font color=\"%s\">\n"
             "%20zd %-.8s [%-.8s] > %s\n"
             "\t\tin %s\n"
             "\t\tat %d line\n"
                "\t</font>\n"
             "</pre>\n",
             logColor, AbsoluteLogLineIndex++, bufferTime, LogLevelMessages[LOG_LVL_TRACE],
             funcName,
             fileName,
             logLine);

    fputs(buffer, LogFiles[logIndex].file);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    fputs("</body>\n"
          "</html>\n", LogFiles[logIndex].file);

    fflush(LogFiles[logIndex].file);
}

FILE* LogBeginDump(const LogSignature sig, const LogLevel logLevel,
             const char* funcName, const char* fileName, const int logLine)
{
    assert(funcName);
    assert(fileName);

    assert(LogFilesCreated);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    int status = LOG_NO_ERRORS;

    char  bufferTime[bufferTimeSize] = "";
    char  buffer[bufferSize]         = "";

    const char* logColor             = "C5E0DC";

    size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    fseek(LogFiles[logIndex].file, TextOffset, SEEK_END);

    time_t rawTime = time(nullptr);
    tm*    curTime = localtime(&rawTime);
    strftime(bufferTime, bufferTimeSize, "%H:%M:%S", curTime);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    snprintf(buffer, bufferSize,
             "<pre>\n"
             "\t<font color=\"%s\">\n"
             "%20zd %-.8s [%-.8s] > %s\n"
             "\t\tin %s\n"
             "\t\tat %d line\n",
             logColor, AbsoluteLogLineIndex++, bufferTime, LogLevelMessages[LOG_LVL_TRACE],
             funcName,
             fileName,
             logLine);

    fputs(buffer, LogFiles[logIndex].file);

    return LogFiles[logIndex].file;
}

void LogEndDump(const LogSignature sig)
{
    size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    fputs("\t</font>\n"
          "</pre>\n"
          "</body>\n"
          "</html>\n", LogFiles[logIndex].file);

    fflush(LogFiles[logIndex].file);
}


void LogAddImage(const LogSignature sig, const char* imagePath)
{
    assert(imagePath);

    size_t logIndex = (size_t)sig;
    assert(logIndex < LogFilesSize);

    FILE* logFile = LogFiles[logIndex].file;

    fseek(logFile, TextOffset, SEEK_END);

    fprintf(logFile, "<img src = \"%s\" height = 600>\n", imagePath);

    fputs("</body>\n"
          "</html>\n", logFile);

    fflush(logFile);
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 