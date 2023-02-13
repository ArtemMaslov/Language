///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
//
// Поддержка компиляции выражений.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 21:18 08.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef COMPILE_EXPR_H
#define COMPILE_EXPR_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Скомпилировать выражение.
 * 
 * @param comp     Указатель на структуру компилятора.
 * @param exprNode Указатель на узел выражения.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileExpression(x86Compiler* const comp, const ExpressionNode* const exprNode);

/**
 * @brief  Скомпилировать унарный оператор.
 * 
 * @param comp       Указатель на структуру компилятора.
 * @param unOperNode Указатель на узел унарного оператора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileUnaryOperator(x86Compiler* const comp, const UnaryOperatorNode* const unOperNode);

/**
 * @brief  Скомпилировать оператор логического отрицания.
 * 
 * @param comp       Указатель на структуру компилятора.
 * @param unOperNode Указатель на узел оператора логического отрицания.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileNotOperator(x86Compiler* const comp, const UnaryOperatorNode* const unOperNode);

/**
 * @brief  Скомпилировать бинарный оператор.
 * 
 * @param comp        Указатель на структуру компилятора.
 * @param binOperNode Указатель на узел бинарного оператора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileBinaryOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode);

/**
 * @brief  Скомпилировать оператор "логическое и".
 * 
 * @param comp        Указатель на структуру компилятора.
 * @param binOperNode Указатель на узел оператора "логическое и".
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileAndOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode);

/**
 * @brief  Скомпилировать оператор "логическое или".
 * 
 * @param comp        Указатель на структуру компилятора.
 * @param binOperNode Указатель на узел оператора "логическое или".
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileOrOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode);

/**
 * @brief  Скомпилировать постоянное число с плавающей точкой.
 * 
 * @param comp   Указатель на структуру компилятора.
 * @param number Указатель на узел числа.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileNumber(x86Compiler* const comp, const double number);

/**
 * @brief  Скомпилировать переменную. 
 * 
 * Используется одновременно и для вызова, и для определения.
 * 
 * @param comp    Указатель на структуру компилятора.
 * @param varNode Указатель на узел переменной.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileVariable(x86Compiler* const comp, const VariableNode* const varNode);

/**
 * @brief  Скомпилировать оператор сравнения.
 * 
 * @param comp        Указатель на структуру компилятора.
 * @param binOperNode Указатель на узел оператора сравнения.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileCompareOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !COMPILE_EXPR_H