///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
//
// Поддержка компиляции функций.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 21:59 08.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef COMPILE_FUNCT_H
#define COMPILE_FUNCT_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Скомпилировать определение функции.
 * 
 * @param comp      Указатель на структуру компилятора.
 * @param functNode Указатель на узел определения функции.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileFunctDef(x86Compiler* const comp, const FunctDefNode* const functNode);

/**
 * @brief  Скомпилировать вызов функции.
 * 
 * @param comp          Указатель на структуру компилятора.
 * @param functCallNode Указатель на узел вызова функции.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileFunctCall(x86Compiler* const comp, const FunctCallNode* const functCallNode);

/**
 * @brief  Скомпилировать встроенную в язык функцию.
 * 
 * @param comp          Указатель на структуру компилятора.
 * @param functCallNode Указатель на узел вызова функции.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileInstrinsicFunction(x86Compiler* const comp, FunctCallNode* const functCallNode);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !COMPILE_FUNCT_H