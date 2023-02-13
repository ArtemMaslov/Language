///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль операционно-зависимых макро определений.
// 
// Версия: 1.0.0.1
// Дата последнего изменения: 11:13 03.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#if defined(WINDOWS)

#define FUNCT_SIG __FUNCSIG__
#define TARGET_OS "Windows"

#elif defined(LINUX)

#define FUNCT_SIG __PRETTY_FUNCTION__
#define TARGET_OS "Linux"

#else

#define FUNCT_SIG "null"
#define TARGET_OS "null"
#error Undefined target os

#endif