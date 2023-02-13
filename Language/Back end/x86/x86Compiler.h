///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 19:45 13.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef X86_COMPILER_H
#define X86_COMPILER_H

#include "../../AST/AST.h"
#include "../../Modules/ErrorsHandling.h"

#include "compiler_config.h"
#include "asm_representation.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Структура компилятора для архитектуры Intel x86 (32-разрядная).
struct x86Compiler
{
	/// Номер метки в ассемблере. Используется для того, чтобы все метки имели разные имена.
	size_t LabelIndex;

	/// Указатель на входное АСД.
	AST* Ast;
	/// Ассемблерное представление программы.
	AsmRepresentation AsmRep;

	/// Узел функции, которая сейчас компилируется.
	const FunctDefNode* CurrentFunction;

	/// Информация о текущих локальных переменных функции.
	/// Все переменные доступны внутри процедуры.
	/// Используется для того, чтобы зарезервировать память в стеке для локальных переменных.
	IdentifierTable CurrentVariablesScope;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Конструктор компилятора x86. 
 * 
 * @param comp Указатель на структуру компилятора.
 * @param ast  Указатель на входное АСД программы.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus x86CompilerConstructor(x86Compiler* const comp, AST* const ast);

/**
 * @brief  Деструктор компилятора x86.
 * 
 * @param comp Указатель на структуру компилятора.
 */
void x86CompilerDestructor(x86Compiler* const comp);

/**
 * @brief  Скомпилировать программу во внутреннее ассемблерное представление.
 * 
 * @param comp Указатель на структуру компилятора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus x86CompileToAsm(x86Compiler* const comp);

/**
 * @brief  Преобразовать ассемблер в байт-код процессора.
 * 
 * После выполнения этой функции команды ещё не лежат последовательно в буфере.
 * 
 * @param comp Указатель на структуру компилятора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus x86TranslateToByteCode(x86Compiler* const comp);

/**
 * @brief  Сформировать из закодированных команд исполняемую в реальном времени программу.
 * 
 * @param comp Указатель на структуру компилятора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus x86CompileJitBuffer(x86Compiler* const comp);

/**
 * @brief  Запустить скомпилированную программу.
 * 
 * @param comp Указатель на структуру компилятора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus x86RunJIT(x86Compiler* const comp);

/**
 * @brief  Вывести состояние скомпилированного JIT-буфера в указанный файл.
 * 
 * @param comp     Указатель на структуру компилятора.
 * @param fileName Имя выходного файла.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus x86DumpJIT(x86Compiler* const comp, const char* const fileName);

/**
 * @brief  Создать листинг файл.
 * 
 * @param comp     Указатель на структуру компилятора.
 * @param fileName Имя выходного файла.
 * @param type     Тип файла листинга. Можно выбрать, выводить ли в файл:
 *                     - Ассемблерные команды текстом.
 *                     - Байт-код команды.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus x86WriteListingFile(const x86Compiler* const comp, const char* const fileName, const AsmListingType type);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !X86_COMPILER_H