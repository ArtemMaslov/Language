#ifndef LOGS_H
#define LOGS_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
/// ������ ��� �������� ������-����� ������ ���������.
/// 
/// �������������:
///     1. �������� ����� "Logs.h", "Logs.cpp", "LogsConfig.h" � ������.
/// 
///     2. ���������� ������������ ���� "Logs.h".
/// 
///     3. � main() � ����� ������ ������� LogsConstructor(), � ����� LogsDestructor().
///            LogsConstructor() - �������������� ����������� ������ �����.
///            LogsDestructor()  - ��������� ����� �����.
/// 
///     4. � ���� "<ModuleName>Logs.h" �������� ������� � ������������ � ��������� ��� ��������� ������������.
/// 
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#include <stdio.h>

#include "LogsConfig.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

/// ������ ������ ������ �����.
enum LogError
{
    /// ������ ���.
    LOG_NO_ERRORS           = 0,
    /// ������ �������� ����� �����.
    LOG_ERR_FILE_OPENING    = 1 << 0,
    /// ���������� ����� ��������� ������������.
    LOG_ERR_MEMORY          = 1 << 1,
};

/// ������ ������������.
enum LogLevel
{
    /// ������� ������ �������.
    LOG_LVL_TRACE   = 0,

    /// ��������� ��� �������.
    /// ��������� � ������.
    LOG_LVL_DEBUG   = 1,

    /// ��������� � ���� ���������� ���������.
    /// �������� � ������.
    LOG_LVL_INFO    = 2,

    /// ��������� � ��������������.
    /// � ���� ���������� ��������� �������� ��������, ������� � ���������� ������ �� ������ ���������.
    /// �������� �� ��������, �� ����� �������� �� ���������. ��������� ��������� ��� ����������.
    LOG_LVL_WARNING = 3,

    /// ��������� �� ������.
    /// ��������� ������ ����� �������� ���������� ������, � ������� ��������� ������.
    /// � ��������� ��������� ��������� ���� ������.
    LOG_LVL_ERROR   = 4,
};

/**
 * @brief ��������� ����� ��������� ����������, � ����� ���� ����� �������� ���������.
 * 
 *        ��������� �������� ������ � ������� ������, ������� 0 <= ������ < LogFilesSize
*/
enum LogSignature
{
    /// ����� ��� ����. �������� ����� ���������� � ���� ���������� ���������.
    LOG_SIG_GENERAL    = 0,
    /// ��� ������ ���-�������.
    LOG_SIG_HASH_TABLE = 1,
    /// ��� ������ ����������� ������.
    LOG_SIG_LIST       = 2,
};

enum LogTraceLevel
{
    /// ������������ / �����������. 
    LOG_TRC_CTOR   = 0,

    /// �������� ������� ������ ������.
    /// ������������� � <ModuleName>.cpp.
    LOG_TRC_FUNC_0 = 1,

    /// �������������� ������� ������.
    /// ������������� � ��������������� ������ ������.
    LOG_TRC_FUNC_1 = 2,

    /// ��������������� ������� ������.
    /// ����� ����������, ������� ����� ���������� ���� �����.
    LOG_TRC_FUNC_2 = 3,

    /// ������������, �����.
    LOG_TRC_VERIFY = 4,
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#define LOG_F_ERR(format, ...) \
    LogFLine(LOG_LVL_ERROR, LOG_SIG_GENERAL, LOG_GLOBAL_DUBLICATE_TO_CONSOLE, format, __VA_ARGS__)

#define LOG_ERR(message) \
    LogLine(message, LOG_LVL_ERROR, LOG_SIG_GENERAL, LOG_GLOBAL_DUBLICATE_TO_CONSOLE)

#define LOG_ERR_MEMORY \
    LOG_ERR("������ ��������� ������")

#define LOG_DBG(message) \
    LogLine(message, LOG_LVL_DEBUG, LOG_SIG_GENERAL, true)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

/**
 * @brief  ������������� ������-�����.
 * 
 * @return LogError. LOG_NO_ERRORS, ���� �� ���� ������.
*/
int LogsConstructor();

/**
 * @brief ��������� ���� �����.
*/
void LogsDestructor();

/**
 * @brief                    ��������� ��������� � ���� �����.
 * 
 * @param message            ���������.
 * @param logLevel           ������� ������������.
 * @param sig                ��������� ����� �����.
 * @param dublicateToConsole �������������� ��������� � �������.
 * @param funcName           ��� �������, � ������� ���������� LogLine. __FUNCSIG__.
 * @param fileName           ��� �����, � ������� ���������� �������. __FILE__.
 * @param logLine            ������, � ������� ���������� LogLine. __LINE__.
*/
void LogLine(const char* message, const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* funcName, const char* fileName, const int logLine);

/**
 * @brief                    ��������� ��������������� � ������� printf ��������� � ���� �����.
 *
 * @param format             ��������� ������.
 * @param logLevel           ������� ������������.
 * @param sig                ��������� ����� �����.
 * @param dublicateToConsole �������������� ��������� � �������.
 * @param funcName           ��� �������, � ������� ���������� LogLine. __FUNCSIG__.
 * @param fileName           ��� �����, � ������� ���������� �������. __FILE__.
 * @param logLine            ������, � ������� ���������� LogLine. __LINE__.
 * @param ...                ������ ���������� ��������� ������. ������ ��������������� printf.
*/
void LogFLine(const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* funcName, const char* fileName, const int logLine, const char* format, ...);

/**
 * @brief          ��������� ��������� � ����������� � ���� �����.
 * @param sig      ��������� �����.
 * @param funcName ��� �������, � ������� ���������� LogLine. __FUNCSIG__.
 * @param fileName ��� �����, � ������� ���������� �������. __FILE__.
 * @param logLine  ������, � ������� ���������� LogLine. __LINE__.
*/
void LogTrace(const LogSignature sig,
               const char* funcName, const char* fileName, const int logLine);

/**
 * @brief          ������ ���������� ��������� ������ � ���� �����.
 * 
 * @param sig      ��������� �����.
 * @param logLevel ������� ������������.
 * @param funcName ��� �������, � ������� ���������� LogLine. __FUNCSIG__.
 * @param fileName ��� �����, � ������� ���������� �������. __FILE__.
 * @param logLine  ������, � ������� ���������� LogLine. __LINE__.
 * 
 * @usage
 *        FILE* log = LogBeginDump();
 *        fputs(dump, log);
 *        LogEndDump();
*/
FILE* LogBeginDump(const LogSignature sig, const LogLevel logLevel,
             const char* funcName, const char* fileName, const int logLine);

/**
 * @brief     ��������� ���������� ��������� ������ � ���� �����.
 * 
 * @param sig ��������� �����.
*/
void LogEndDump(const LogSignature sig);

/**
 * @brief           �������� ����������� � ���� �����.
 *
 * @param sig       ��������� �����.
 * @param imagePath ���� � �����������.
*/
void LogAddImage(const LogSignature sig, const char* imagePath);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#define LogLine(message, logLevel, sig, dublicateToConsole) \
    LogLine(message, logLevel, sig, dublicateToConsole, __FUNCSIG__, __FILE__, __LINE__)

#define LogFLine(logLevel, sig, dublicateToConsole, format, ...) \
    LogFLine(logLevel, sig, dublicateToConsole, __FUNCSIG__, __FILE__, __LINE__, format, __VA_ARGS__)

#define LogTrace(sig) \
    LogTrace(sig, __FUNCSIG__, __FILE__, __LINE__)

#define LogBeginDump(sig, lvl) \
    LogBeginDump(sig, lvl, __FUNCSIG__, __FILE__, __LINE__)

#define LogEndDump(sig) \
    LogEndDump(sig)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#endif // !LOGS_H
