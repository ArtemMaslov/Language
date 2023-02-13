///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
//
// Второстепенные функции.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 21:18 08.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef COMPILE_AUX_H
#define COMPILE_AUX_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Скомпилировать блок инструкций.
 * 
 * @param comp       Указатель на структуру компилятора.
 * @param instrBlock ExtArray<InstructionNode> - блок инструкций.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileInstrBlock(x86Compiler* const comp, const ExtArray* const instrBlock);

/**
 * @brief  Скомпилировать конструкционные узлы.
 * 
 * @param comp Указатель на структуру компилятора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus CompileConstrNodes(x86Compiler* const comp);

/**
 * @brief  Получить относительное смещение адреса переменной. 
 * 
 * @param comp                 Указатель на структуру компилятора.
 * @param varId                Идентификатор переменной.
 * @param outOffset            Выходное смещение.
 * @param outVarInitialization Выходной флаг. Является ли использование переменной
 *                             инициализацией (true) или получением её значения (false).
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus IdentifierGetMemoryOffset(x86Compiler* const comp, const size_t varId, 
                                        int* const outOffset, bool* const outVarInitialization = nullptr);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !COMPILE_AUX_H