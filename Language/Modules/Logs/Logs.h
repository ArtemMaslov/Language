#ifndef LOGS_H
#define LOGS_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
/// Модуль для создания файлов-логов работы программы.
/// 
/// Использование:
///     1. Добавить файлы "Logs.h", "Logs.cpp", "LogsConfig.h" в проект.
/// 
///     2. Подключить заголовочный файл "Logs.h".
/// 
///     3. В main() в самом начале вызвать LogsConstructor(), в конце LogsDestructor().
///            LogsConstructor() - инициализирует дескрипторы файлов логов.
///            LogsDestructor()  - закрывает файлы логов.
/// 
///     4. В файл "<ModuleName>Logs.h" добавить дефайны с трассировкой и макросами над функциями логгирования.
/// 
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#include <stdio.h>

#include "LogsConfig.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

/// Ошибки работы модуля логов.
enum LogError
{
    /// Ошибок нет.
    LOG_NO_ERRORS           = 0,
    /// Ошибка открытия файла логов.
    LOG_ERR_FILE_OPENING    = 1 << 0,
    /// Внутренний буфер сообщения переполнился.
    LOG_ERR_MEMORY          = 1 << 1,
};

/// Уровни логгирования.
enum LogLevel
{
    /// Порядок вызова функций.
    LOG_LVL_TRACE   = 0,

    /// Сообщения для отладки.
    /// Убираются в релизе.
    LOG_LVL_DEBUG   = 1,

    /// Сообщения о ходе выполнения программы.
    /// Остаются в релизе.
    LOG_LVL_INFO    = 2,

    /// Сообщение о предупреждении.
    /// В ходе выполнения программы возникла ситуация, которая в нормальном режиме не должна возникать.
    /// Ситуация не критична, но может повлиять на результат. Программа продолжит своё выполнение.
    LOG_LVL_WARNING = 3,

    /// Сообщения об ошибке.
    /// Программа должна после прервать выполнения модуля, в котором произошла ошибка.
    /// В частности полностью завершить свою работу.
    LOG_LVL_ERROR   = 4,
};

/**
 * @brief Сигнатура файла позволяет определить, в какой файл будут записано сообщение.
 * 
 *        Значением является индекс в массиве файлов, поэтому 0 <= индекс < LogFilesSize
*/
enum LogSignature
{
    /// Общий лог файл. Содержит общую информацию о ходе выполнении программы.
    LOG_SIG_GENERAL    = 0,
    /// Лог модуля хеш-таблицы.
    LOG_SIG_HASH_TABLE = 1,
    /// Лог модуля двусвязного списка.
    LOG_SIG_LIST       = 2,
};

enum LogTraceLevel
{
    /// Конструкторы / деструкторы. 
    LOG_TRC_CTOR   = 0,

    /// Основные функции работы модуля.
    /// Располагаются в <ModuleName>.cpp.
    LOG_TRC_FUNC_0 = 1,

    /// Второстепенные функции модуля.
    /// Располагаются в вспомогательных файлах модуля.
    LOG_TRC_FUNC_1 = 2,

    /// Вспомогательные функции модуля.
    /// Часто вызываются, поэтому могут замусорить файл логов.
    LOG_TRC_FUNC_2 = 3,

    /// Верификаторы, дампы.
    LOG_TRC_VERIFY = 4,
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#define LOG_F_ERR(format, ...) \
    LogFLine(LOG_LVL_ERROR, LOG_SIG_GENERAL, LOG_GLOBAL_DUBLICATE_TO_CONSOLE, format, __VA_ARGS__)

#define LOG_ERR(message) \
    LogLine(message, LOG_LVL_ERROR, LOG_SIG_GENERAL, LOG_GLOBAL_DUBLICATE_TO_CONSOLE)

#define LOG_ERR_MEMORY \
    LOG_ERR("Ошибка выделения памяти")

#define LOG_DBG(message) \
    LogLine(message, LOG_LVL_DEBUG, LOG_SIG_GENERAL, true)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

/**
 * @brief  Инициализация файлов-логов.
 * 
 * @return LogError. LOG_NO_ERRORS, если не было ошибок.
*/
int LogsConstructor();

/**
 * @brief Закрывает файл логов.
*/
void LogsDestructor();

/**
 * @brief                    Сохранить сообщение в файл логов.
 * 
 * @param message            Сообщение.
 * @param logLevel           Уровень логгирования.
 * @param sig                Сигнатура файла логов.
 * @param dublicateToConsole Продублировать сообщение в консоль.
 * @param funcName           Имя функции, в которой вызывается LogLine. __FUNCSIG__.
 * @param fileName           Имя файла, в котором вызывается функция. __FILE__.
 * @param logLine            Строка, в которой вызывается LogLine. __LINE__.
*/
void LogLine(const char* message, const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* funcName, const char* fileName, const int logLine);

/**
 * @brief                    Сохранить форматированное с помощью printf сообщение в файл логов.
 *
 * @param format             Форматная строка.
 * @param logLevel           Уровень логгирования.
 * @param sig                Сигнатура файла логов.
 * @param dublicateToConsole Продублировать сообщение в консоль.
 * @param funcName           Имя функции, в которой вызывается LogLine. __FUNCSIG__.
 * @param fileName           Имя файла, в котором вызывается функция. __FILE__.
 * @param logLine            Строка, в которой вызывается LogLine. __LINE__.
 * @param ...                Список аргументов форматной строки. Должны соответствовать printf.
*/
void LogFLine(const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* funcName, const char* fileName, const int logLine, const char* format, ...);

/**
 * @brief          Сохранить сообщение о трассировке в файл логов.
 * @param sig      Сигнатура файла.
 * @param funcName Имя функции, в которой вызывается LogLine. __FUNCSIG__.
 * @param fileName Имя файла, в котором вызывается функция. __FILE__.
 * @param logLine  Строка, в которой вызывается LogLine. __LINE__.
*/
void LogTrace(const LogSignature sig,
               const char* funcName, const char* fileName, const int logLine);

/**
 * @brief          Начать сохранение состояния модуля в файл логов.
 * 
 * @param sig      Сигнатура файла.
 * @param logLevel Уровень логгирования.
 * @param funcName Имя функции, в которой вызывается LogLine. __FUNCSIG__.
 * @param fileName Имя файла, в котором вызывается функция. __FILE__.
 * @param logLine  Строка, в которой вызывается LogLine. __LINE__.
 * 
 * @usage
 *        FILE* log = LogBeginDump();
 *        fputs(dump, log);
 *        LogEndDump();
*/
FILE* LogBeginDump(const LogSignature sig, const LogLevel logLevel,
             const char* funcName, const char* fileName, const int logLine);

/**
 * @brief     Завершить сохранение состояния модуля в файл логов.
 * 
 * @param sig Сигнатура файла.
*/
void LogEndDump(const LogSignature sig);

/**
 * @brief           Добавить изображение в файл логов.
 *
 * @param sig       Сигнатура файла.
 * @param imagePath Путь к изображению.
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
