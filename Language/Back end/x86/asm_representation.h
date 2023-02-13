///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль ассемблерного представления для архитектуры Intel x86 (32-разряда).
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 22:09 13.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef ASM_REPRESENTATION_H
#define ASM_REPRESENTATION_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <stdint.h>
#include "../../Modules/ExtArray/ExtArray.h"
#include "../../Modules/ExtHeap/ExtHeap.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include "asm_defs.h"
#include "asm_labels.h"

/// Общая структура ассемблерного представления.
struct AsmRepresentation
{
    /**
     * @brief Массив ассемблерных команд.
     * ExtArray<AsmCmd>
    */
	ExtArray Commands;
	/// Массив символьных названий меток.
	ExtHeap  Labels;

    /// Таблица меток.
	AsmLabelTable LabelTable;

    /// Размер буфера программы.
	size_t JitBufferSize;
    /// Текущий указатель на текущую транслируемую команду.
	size_t CurCmdOffset;

    /// Буфер команд.
	uint8_t* JitBuffer;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Начальная вместимость кучи в байтах.
static const size_t AsmRepLabelsDefaultCapacity = 4096;
/// Начальное количество ассемблерных команд в программе.
static const size_t AsmRepCommandsDefaultCount  = 4096;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Конструктор ассемблерного представления.
 * 
 * В случае ошибки не изменяет состояние структуры.
 * 
 * @param asmRep Указатель на структуру ассемблерного представления.
 * 
 * @return ProgramStatus::Ok, если конструктор выполнен успешно.
 */
ProgramStatus AsmRepConstructor(AsmRepresentation* const asmRep);

/**
 * @brief  Деструктор ассемблерного представления.
 * 
 * @param asmRep Указатель на структуру ассемблерного представления.
 */
void AsmRepDestructor(AsmRepresentation* const asmRep);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Транслирует ассемблерные команды в байт-код процессора.
 * 
 * @param asmRep Указатель на структуру ассемблерного представления.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus AsmTranslateCmdsToByteCode(AsmRepresentation* const asmRep);

/**
 * @brief  Создать листинг файл.
 * 
 * @param asmRep   Указатель на структуру ассемблерного представления.
 * @param fileName Имя выходного файла.
 * @param type     Тип файла листинга. Можно выбрать, выводить ли в файл:
 *                     - Ассемблерные команды текстом.
 *                     - Байт-код команды.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus AsmWriteListingFile(const AsmRepresentation* const asmRep, const char* const fileName, const AsmListingType type);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Создать узел ассемблерной команды.
 * 
 * @param asmRep Указатель на структуру ассемблерного представления.
 * @param type   Тип ассемблерной команды.
 * @param arg1   Первый аргумент.
 * @param arg2   Второй аргумент.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus AsmCreateCommand(AsmRepresentation* const asmRep, const AsmCmdType type, const AsmArg* const arg1 = nullptr, const AsmArg* const arg2 = nullptr);

/**
 * @brief  Создать аргумент - целое число.
 * 
 * @param arg Указатель на выходной аргумент команды.
 * @param imm Целое число.
 * 
 * @return Аргумент ассемблерной команды.
 */
inline AsmArg* AsmCreateArgImm(AsmArg* const arg, const int imm)
{
	assert(arg);

	arg->Type = AsmArgType::Imm;
	arg->Imm  = imm;

	return arg;
}

/**
 * @brief  Создать аргумент - адрес памяти = значение регистра + целое смещение.
 * 
 * @param arg Указатель на выходной аргумент команды.
 * @param reg Регистр.
 * @param imm Целое смещение.
 * 
 * @return Аргумент ассемблерной команды.
 */
inline AsmArg* AsmCreateArgMem(AsmArg* const arg, const AsmRegType reg, const int imm)
{
	assert(arg);

	arg->Type = AsmArgType::Mem;
	arg->Imm  = imm;
	arg->Reg  = reg;

	return arg;
}

/**
 * @brief  Создать аргумент - регистр.
 * 
 * @param arg Указатель на выходной аргумент команды.
 * @param reg Регистр.
 * 
 * @return Аргумент ассемблерной команды.
 */
inline AsmArg* AsmCreateArgReg(AsmArg* const arg, const AsmRegType reg)
{
	assert(arg);

	arg->Type = AsmArgType::Reg;
	arg->Reg  = reg;

	return arg;
}

/**
 * @brief  Создать аргумент - метка.
 * 
 * @param arg   Указатель на выходной аргумент команды.
 * @param label Имя метки.
 * 
 * @return Аргумент ассемблерной команды.
 */
inline AsmArg* AsmCreateArgLabel(AsmArg* const arg, const char* const label)
{
	assert(arg);
	assert(label);

	arg->Type  = AsmArgType::Label;
	arg->Label = label;

	return arg;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Получить название команды по её типу.
 * 
 * @param type Тип команды.
 * 
 * @return Название команды.
 */
const char* AsmGetCommandName(const AsmCmdType type);

/**
 * @brief  Получить название регистра по его типу.
 * 
 * @param type Тип регистра.
 * 
 * @return Название регистра.
 */
const char* AsmGetRegName(const AsmRegType type);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !ASM_REPRESENTATION_H