///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль для создания файлов-логов работы программы.
// 
// Использование:
// 1. Добавить директорию "Modules/Logs" в проект.
// 
// 2. В main() в самом начале вызвать LogsConstructor(), в конце LogsDestructor().
//    LogsConstructor() - инициализирует дескрипторы файлов логов.
//    LogsDestructor()  - закрывает файлы логов.
// 
// 3. В файлы, в которых нужно использовать логи, подключить заголовочный файл "Logs.h".
// 
// 4. Доступные функции (параметры и описание см. ниже):
//     4.1.  LogLine
//     4.2.  LogFLine
//     4.3.  LogTrace
//     4.4.  LogBeginDump
//     4.5.  LogEndDump
//     4.6.  LogAddImage
//    Макросы, облегчающие работу с логами. Добавление записей:
//     4.7.  LOG_F_ERR
//     4.8.  LOG_ERR
//     4.9.  LOG_ERR_MEMORY
//     4.10. LOG_F_WRN
//     4.11. LOG_WRN
//     4.12. LOG_F_INFO
//     4.13. LOG_INFO
//     4.14. LOG_F_DBG
//     4.15. LOG_DBG
//    Макросы, облегчающие работу с логами. Трассировка сообщений:
//     4.16. TRACE_CTOR
//     4.17. TRACE_FUNCT0
//     4.18. TRACE_FUNCT1
//     4.19. TRACE_FUNCT2
//     4.20. TRACE_VERIFY
//     4.21. TRACE_ERROR
// 
// 5. Настройка конфигурации логов:
//     5.1. Файл "LogsConfig.h" (входит в "Logs.h"):
//          * Изменить директорию сохранения файлов логов;
//          * Включить/отключить глобальный вывод сообщений логов в консоль;
//          * Включить/отключить трассировку функций определенного типа;
//     5.2. Файл "logs_config_private.h" (этот файл вручную никуда подключать не нужно):
//          * Изменить цвета текста, заднего фона LOG_*_COLOR.
//          * Изменить размер текста LOG_TEXT_SIZE.
//          * Изменить кодировку записанного текста в файл логов LOG_CODEPAGE.
//          * Изменить расстояние между записями LOG_LINE_HEIGHT.
//          * Изменить размер буфера форматирования текста BufferSize.
// 
// 6. Если нужно использовать несколько файлов логов, то нужно:
//     6.1. Добавить новую сигнатуру в LogSignature в файле "Logs.h".
//     6.2. Вызвать конструктор логов в файле "Logs.cpp" в функции LogsConstructor(). 
//          !В конце слова Logs 's', не перепутать с LogConstructor().
// 
// 7. Если нужно отключить поддержку логов в модуле, можно добавить в модуль файл "Modules/DisableLogs.h".
//    Более подробное описание смотри в заголовке этого файла.
// 
// Версия: 1.0.1.3
// Дата последнего изменения: 14:22 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>

#include "../TargetOS.h"
#include "LogsConfig.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Особые ситуации при работе модуля логов.
enum class LogError
{
    /// Ошибок нет.
    NoErrors,
    /// Ошибка открытия файла логов.
    FileOpen,
    /// Внутренний буфер сообщений переполнился.
    Memory
};

/// Уровни логированюия.
enum class LogLevel
{
    /// Порядок вызова функций.
    Trace,

    /// Сообщения для отладки.
    /// Убираются в релизе.
    Debug,

    /// Сообщения о ходе выполнения программы.
    /// Остаются в релизе.
    Info,

    /// Сообщение о предупреждении.
    /// В ходе выполнения программы возникла ситуация, которая в нормальном режиме не должна возникать.
    /// Ситуация не критична, но может повлиять на результат. Программа продолжит своё выполнение.
    Warning,

    /// Сообщения об ошибке.
    /// Ошибка в модуле. Сильно нарушает работу программы.
    /// Либо модуль отключается, и выполнение продолжается. Либо работа программы завершается.
    Error
};

/**
 * @brief Сигнатура файла позволяет определить, в какой файл будут записано сообщение.
 * 
 * Значением является индекс в массиве файлов, поэтому 0 <= индекс < LogFilesSize
*/
enum class LogSignature
{
    /// Общий лог файл. Содержит общую информацию о ходе выполнении программы.
    General,

    ///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
    // Добавить ещё одну сигнатуру файла логов здесь.
    ///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

    /// Сигнатура для определения количества файлов логов.
    Reserved
};

/// Уровни трассировки.
enum class LogTrace
{
    /// Конструкторы / деструкторы. 
    Ctor   = 0,

    /// Основные функции работы модуля.
    /// Вызывается из внешних модулей и выполняет основные операции.
    Funct0 = 1,

    /// Вспомогательные функции модуля.
    /// Вызываются внутри модуля основными функциями.
    Funct1 = 2,

    /// Служебные функции модуля.
    /// Часто вызываются в ходе выполнения основной операции, обычно короткие.
    /// Трассировка таких функций замусорит лог файл.
    Funct2 = 3,

    /// Верификаторы, дампы.
    Verify = 4,

    /// Обратная трассировка ошибки.
    Error  = 5,
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Форматированное сообщение об ошибке типа [ERROR] в файл General.
 * 
 * @param  format Текст сообщения. Строка форматирования printf.
 * @param  ...    Аргументы строки форматирования.
 * 
 * @return void.
*/
#define LOG_F_ERR(format, ...) \
    LogFLine(LogLevel::Error, LogSignature::General, LOG_GLOBAL_DUBLICATE_TO_CONSOLE, format, __VA_ARGS__)

/**
 * @brief  Сообщение об ошибке типа [ERROR] в файл General.
 * 
 * @param  message Текст сообщения.
 * 
 * @return void.
*/
#define LOG_ERR(message) \
    LogLine(message, LogLevel::Error, LogSignature::General, LOG_GLOBAL_DUBLICATE_TO_CONSOLE)

/**
 * @brief  Сообщение об ошибке выделения памяти типа [ERROR] в файл General.
 * 
 * @param  memSize Размер запрошенной динамической памяти в байтах.
 * 
 * @return void.
*/
#define LOG_ERR_MEMORY(memSize) \
    LOG_F_ERR("Ошибка выделения памяти. Запрошено %zd байт.", memSize)

/**
 * @brief  Добавить трассировку вызова конструктора/деструктора.
 * 
 * @return void.
*/
#define TRACE_CTOR() \
    LogAddTrace(LogSignature::General, LogTrace::Ctor)

/**
 * @brief  Добавить трассировку вызова основной функции модуля.
 *         Основная функция вызывается из внешних модулей и выполняет основные операции.
 * 
 * @return void.
*/
#define TRACE_FUNCT0() \
    LogAddTrace(LogSignature::General, LogTrace::Funct0)

/**
 * @brief  Добавить трассировку вызова второстепенной функции модуля.
 *         Второстепенная функция вызывается внутри модуля основными функциями.
 * 
 * @return void.
*/
#define TRACE_FUNCT1() \
    LogAddTrace(LogSignature::General, LogTrace::Funct1)

/**
 * @brief  Добавить трассировку вызова вспомогательной функции модуля.
 *         Вспомогательная функция часто вызывается в ходе выполнения основной операции, часто короткая.
 *         Трассировка такой функции замусорит лог файл.
 * 
 * @return void.
*/
#define TRACE_FUNCT2() \
    LogAddTrace(LogSignature::General, LogTrace::Funct2)

/**
 * @brief  Добавить трассировку вызова верификаторов и дампов.
 * 
 * @return void.
*/
#define TRACE_VERIFY() \
    LogAddTrace(LogSignature::General, LogTrace::Verify)

/**
 * @brief  Обратная трассировка при возникновении ошибки. Для получения стека вызовов.
 * 
 * @usage
 * int SomeFunct()
 * {
 *      if (...)
 *      {
 *          LOG_ERR("Error message");
 *          return Error;
 *      }
 * }
 * 
 * int AnotherFunct()
 * {
 *      int status = SomeFunct();
 *      if (status == Error)
 *      {
 *          TRACE_ERROR();
 *          return status;
 *      }
 * }
 * 
 * @return void.
*/
#define TRACE_ERROR() \
    LogAddTrace(LogSignature::General, LogTrace::Error)

/**
 * @brief  Сообщение типа [DEBUG] в файл General.
 * 
 * @param  format Текст сообщения. Строка форматирования printf.
 * @param  ...    Аргументы строки форматирования.
 * 
 * @return void.
*/
#define LOG_F_DBG(format, ...) \
    LogFLine(LogLevel::Debug, LogSignature::General, LOG_GLOBAL_DUBLICATE_TO_CONSOLE, format, __VA_ARGS__)

/**
 * @brief  Сообщение типа [DEBUG] в файл General.
 * 
 * @param  message Текст сообщения.
 * 
 * @return void.
*/
#define LOG_DBG(message) \
    LogLine(message, LogLevel::Debug, LogSignature::General, true)

/**
 * @brief  Сообщение типа [WARNING] в файл General.
 * 
 * @param  format Текст сообщения. Строка форматирования printf.
 * @param  ...    Аргументы строки форматирования.
 * 
 * @return void.
*/
#define LOG_F_WRN(format, ...) \
    LogFLine(LogLevel::Warning, LogSignature::General, LOG_GLOBAL_DUBLICATE_TO_CONSOLE, format, __VA_ARGS__)

/**
 * @brief  Сообщение типа [WARNING] в файл General.
 * 
 * @param  message Текст сообщения.
 * 
 * @return void.
*/
#define LOG_WRN(message) \
    LogLine(message, LogLevel::Warning, LogSignature::General, true)

/**
 * @brief  Сообщение типа [INFO] в файл General.
 * 
 * @param  format Текст сообщения. Строка форматирования printf.
 * @param  ...    Аргументы строки форматирования.
 * 
 * @return void.
*/
#define LOG_F_INFO(format, ...) \
    LogFLine(LogLevel::Info, LogSignature::General, LOG_GLOBAL_DUBLICATE_TO_CONSOLE, format, __VA_ARGS__)

/**
 * @brief  Сообщение типа [INFO] в файл General.
 * 
 * @param  message Текст сообщения.
 * 
 * @return void.
*/
#define LOG_INFO(message) \
    LogLine(message, LogLevel::Info, LogSignature::General, true)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Инициализация файлов-логов.
 * 
 * @return LogError. LOG_NO_ERRORS, если не было ошибок.
*/
LogError LogsConstructor();

/**
 * @brief Закрывает файл логов.
*/
void LogsDestructor();

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

void LogLine(const char* const message, const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* const funcName, const char* const fileName, const int logLine);

/**
* @brief  Добавить запись в файл логов.
* 
* @param message            Текст сообщения.
* @param logLevel           Уровень логирования.
* @param sig                Сигнатура файла.
* @param dublicateToConsole Если true, то сообщение логов дублируется в консоль.
* 
* @return void.
*/
#define LogLine(message, logLevel, sig, dublicateToConsole) \
    LogLine(message, logLevel, sig, dublicateToConsole, FUNCT_SIG, __FILE__, __LINE__)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

void LogFLine(const LogLevel logLevel, const LogSignature sig, const bool dublicateToConsole,
                const char* const funcName, const char* const fileName, const int logLine, const char* const format, ...);
/**
* @brief  Добавить форматированную запись в файл логов.
* 
* @param logLevel           Уровень логирования.
* @param sig                Сигнатура файла логов.
* @param dublicateToConsole Если true, то сообщение логов дублируется в консоль.
* @param format             Строка форматирования сообщения printf.
* @param ...                Аргументы строки форматирования.
* 
* @return void.
*/
#define LogFLine(logLevel, sig, dublicateToConsole, format, ...) \
    LogFLine(logLevel, sig, dublicateToConsole, FUNCT_SIG, __FILE__, __LINE__, format, __VA_ARGS__)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

void LogAddTrace(const LogSignature sig, const LogTrace trace,
               const char* const funcName, const char* const fileName, const int logLine);

/**
* @brief  Добавить запись о вызове функции.
* 
* @param sig   Сигнатура файла.
* @param trace Тип трассировочного сообщения.
* 
* @return void.
*/
#define LogAddTrace(sig, trace) \
    LogAddTrace(sig, trace, FUNCT_SIG, __FILE__, __LINE__)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

FILE* LogBeginDump(const LogSignature sig, const LogLevel logLevel,
             const char* const funcName, const char* const fileName, const int logLine);

/**
 * @brief  Начать прямую запись в файл логов.
 *
 * Обычно используется для создания дампа.
 * @warning В конце работы с файлом логов нужно вызвать LogEndDump().
 * @warning Пока не будет вызвана LogEndDump() нельзя вызывать другие функции работы
 * с данным файлом логов.
 * 
 * @param sig Сигнатура файла.
 * @param lvl Уровень сообщения. Обычно LogLevel::Debug.
 * 
 * @usage
 * {
 *     FILE* log = LogBeginDump();
 *     fputs(dump, log);
 *     LogEndDump();
 * }
 * 
 * @return FILE*, указатель на дескриптор файла логов.
*/
#define LogBeginDump(sig, lvl) \
    LogBeginDump(sig, lvl, FUNCT_SIG, __FILE__, __LINE__)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Завершить прямую запись в файл логов.
 * 
 * @param sig Сигнатура файла.
 * 
 * @return void.
*/
void LogEndDump(const LogSignature sig);

/**
 * @brief  Добавить изображение в файл логов.
 *
 * @param sig       Сигнатура файла.
 * @param imagePath Путь к изображению.
*/
void LogAddImage(const LogSignature sig, const char* const imagePath);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !LOGS_H
