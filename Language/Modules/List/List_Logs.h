#ifndef LIST_LOGS_H
#define LIST_LOGS_H

#include "..\Logs\Logs.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define LOG_CFG_LIST_TRACE_CTOR   false
#define LOG_CFG_LIST_TRACE_FUNC_0 false
#define LOG_CFG_LIST_TRACE_FUNC_1 false
#define LOG_CFG_LIST_TRACE_FUNC_2 false
#define LOG_CFG_LIST_TRACE_VERIFY false

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#if not (LOG_CFG_GLOBAL_TRACE_CTOR && LOG_CFG_LIST_TRACE_CTOR)
    #undef TRACE_CTOR
    #define TRACE_CTOR()
#endif

#if not (LOG_CFG_GLOBAL_TRACE_FUNC_0 && LOG_CFG_LIST_TRACE_FUNC_0)
    #undef TRACE_FUNCT0
    #define TRACE_FUNCT0()
#endif

#if not (LOG_CFG_GLOBAL_TRACE_FUNC_1 && LOG_CFG_LIST_TRACE_FUNC_1)
    #undef TRACE_FUNCT1
    #define TRACE_FUNCT1()
#endif

#if not (LOG_CFG_GLOBAL_TRACE_FUNC_2 && LOG_CFG_LIST_TRACE_FUNC_2)
    #undef TRACE_FUNCT2
    #define TRACE_FUNCT2()
#endif

#if not (LOG_CFG_GLOBAL_TRACE_VERIFY && LOG_CFG_LIST_TRACE_VERIFY)
    #undef TRACE_VERIFY
    #define TRACE_VERIFY()
#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !LIST_LOGS_H