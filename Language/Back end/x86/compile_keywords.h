///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
//
// Поддержка компиляции ключевых слов.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 21:18 08.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef COMPILE_KEYWORD_H
#define COMPILE_KEYWORD_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Скомпилировать оператор цикла.
 * 
 * @param comp      Указатель на структуру компилятора.
 * @param whileNode Указатель на узел цикла.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileWhile(x86Compiler* const comp, const WhileNode* const whileNode);

/**
 * @brief  Скомпилировать условный оператор.
 * 
 * @param comp   Указатель на структуру компилятора.
 * @param ifNode Указатель на узел условного оператора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileIf(x86Compiler* const comp, const IfNode* const ifNode);

/**
 * @brief  Скомпилировать оператор вывода.
 * 
 * @param comp       Указатель на структуру компилятора.
 * @param outputNode Указатель на узел оператора вывода.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileOutput(x86Compiler* const comp, const OutputNode* const outputNode);

/**
 * @brief  Скомпилировать оператор ввода.
 * 
 * @param comp      Указатель на структуру компилятора.
 * @param inputNode Указатель на узел оператора ввода.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileInput(x86Compiler* const comp, const InputNode* const inputNode);

/**
 * @brief  Скомпилировать оператор возврата.
 * 
 * @param comp       Указатель на структуру компилятора.
 * @param returnNode Указатель на оператор возврата.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileReturn(x86Compiler* const comp, const ReturnNode* const returnNode);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !COMPILE_KEYWORD_H