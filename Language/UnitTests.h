///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Конфигурация модульного тестирования
// 
// Версия: 1.0.0.0
// Дата последнего изменения: 11:23 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef UNIT_TEST_H
#define UNIT_TEST_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Установить false, чтобы скомпилировать основную программу.
/// Установить true,  чтобы запустить тестирование модулей. Также необходимо выбрать 
/// модуль для тестирования ниже.
#define ENABLE_UNIT_TESTS true

// Выбор тестируемых модулей. 1 - включить тестирование, 0 - выключить.
// Одновременно можно проводить тестирование только 1 модуля.
#if ENABLE_UNIT_TESTS
/// Тестирование лексического анализатора.
#define ENABLE_LEXER_TEST                0
#define ENABLE_PARSER_TEST               0
#define ENABLE_X86_COMPILER_TEST_COMPILE 0
#define ENABLE_X86_COMPILER_TEST_RUN     1
#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !UNIT_TEST_H