#ifndef LIST_LOGS_H
#define LIST_LOGS_H

#include "..\Logs\Logs.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

/// Дублирование текста в консоль.
#define LOG_LIST_DUBLICATE_TO_CONSOLE LOG_GLOBAL_DUBLICATE_TO_CONSOLE

/// Конструкторы / деструкторы. 
#define LOG_CFG_LIST_TRACE_CTOR true

/// Основные функции работы модуля.
/// Обычно располагаются в <ModuleName>.cpp.
#define LOG_CFG_LIST_TRACE_FUNC_0 true

/// Второстепенные функции модуля.
/// Располагаются в вспомогательных файлах модуля.
#define LOG_CFG_LIST_TRACE_FUNC_1 true

/// Вспомогательные функции модуля.
/// Часто вызываются, поэтому могут замусорить файл логов.
#define LOG_CFG_LIST_TRACE_FUNC_2 true

/// Верификаторы, дампы.
//#define LOG_CFG_LIST_TRACE_VERIFY true

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#if LOG_CFG_GLOBAL_TRACE_CTOR && LOG_CFG_LIST_TRACE_CTOR
    #define LOG_LIST_TRACE_CTOR LogTrace(LOG_SIG_LIST)
#else 
    #define LOG_LIST_TRACE_CTOR ((void)0)
#endif

#if LOG_CFG_GLOBAL_TRACE_FUNC_0 && LOG_CFG_LIST_TRACE_FUNC_0
    #define LOG_LIST_TRACE_FUNC_0 LogTrace(LOG_SIG_LIST)
#else 
    #define LOG_LIST_TRACE_FUNC_0 ((void)0)
#endif

#if LOG_CFG_GLOBAL_TRACE_FUNC_1 && LOG_CFG_LIST_TRACE_FUNC_1
    #define LOG_LIST_TRACE_FUNC_1 LogTrace(LOG_SIG_LIST)
#else 
    #define LOG_LIST_TRACE_FUNC_1 ((void)0)
#endif

#if LOG_CFG_GLOBAL_TRACE_FUNC_2 && LOG_CFG_LIST_TRACE_FUNC_2
    #define LOG_LIST_TRACE_FUNC_2 LogTrace(LOG_SIG_LIST)
#else 
    #define LOG_LIST_TRACE_FUNC_2 ((void)0)
#endif

#if LOG_CFG_GLOBAL_TRACE_VERIFY && LOG_CFG_LIST_TRACE_VERIFY
    #define LOG_LIST_TRACE_VERIFY LogTrace(LOG_SIG_LIST)
#else 
    #define LOG_LIST_TRACE_VERIFY ((void)0)
#endif

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define LOG_F_LIST_ERR(format, ...) \
    LogFLine(LOG_LVL_ERROR, LOG_SIG_LIST, LOG_LIST_DUBLICATE_TO_CONSOLE, format, __VA_ARGS__)

#define LOG_LIST_ERR(message) \
    LogLine(message, LOG_LVL_ERROR, LOG_SIG_LIST, LOG_LIST_DUBLICATE_TO_CONSOLE)

#define LOG_LIST_ERR_MEMORY \
    LOG_LIST_ERR("Ошибка выделения памяти")

#define LOG_LIST_DBG(message) \
    LogLine(message, LOG_LVL_DEBUG, LOG_SIG_LIST, true)

#define LOG_LIST_BEGIN_DUMP \
    LogBeginDump(LOG_SIG_LIST, LOG_LVL_DEBUG)

#define LOG_LIST_END_DUMP \
    LogEndDump(LOG_SIG_LIST)

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !LIST_LOGS_H