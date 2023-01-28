#ifndef ASM_REPRESENTATION_H
#define ASM_REPRESENTATION_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <stdint.h>
#include "../../Modules/ExtArray/ExtArray.h"
#include "../../Modules/ExtHeap/ExtHeap.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define ASM_CMD(cmd, ...) cmd,
enum class AsmCmdType
{
	label = -2,
	null  = -1,

#include "_asm_cmd_names.inc"
};
#undef ASM_CMD

#define ASM_REG(reg, ...) reg,
enum class AsmRegType
{
	null = -1,

#include "_asm_regs.inc"
};
#undef ASM_REG

enum class AsmArgType
{
	Null  = 0,
	Reg   = 1,
	Mem   = 2,
	Imm   = 3,
	Label = 4,
};

struct AsmArg
{
	AsmArgType  Type;
	AsmRegType  Reg;
	int32_t     Imm;
	const char* Label;
};

struct AsmCode
{
	// Бинарное представление команды. Размер массива вмещает самую длинную команду.
	// Prefixes[0|1|2|3|4] + Opcode[1|2] + MemRM[0|1] + SIB[0|1] + Displacement[0|1|2|4] + Immediate[0|1|2|4].
	uint8_t Code[4 + 2 + 1 + 1 + 4 + 4];
	// Актуальный размер команды в байтах.
	size_t  CodeSize;
};

struct AsmCmd
{
	AsmCmdType Type;

	AsmArg     Arg1;
	AsmArg     Arg2;
			  
	AsmCode    Code;
};

struct AsmCmdDescr
{
	AsmCmdType Type;

	AsmArgType Arg1;
	AsmArgType Arg2;

	AsmCode    Code;
};

struct AsmLabelTable
{
	// ExtArray<AsmLabelNode>
	ExtArray Labels;
	// ExtArray<AsmLabelInsert>
	ExtArray Inserts;
};

const size_t AsmLabelsDefaultCapacity  = 256;
const size_t AsmInsertsDefaultCapacity = 256;

struct AsmLabelNode
{
	const char* LabelName;
	size_t      RelativeOffset;
};

struct AsmLabelInsert
{
	const char* LabelName;

	int* Insert;
};

struct AsmRep
{
	// ExtArray<AsmCmd>
	ExtArray Commands;
	ExtHeap  Labels;

	AsmLabelTable LabelTable;

	size_t JitBufferSize;
	size_t CurCmdOffset;

	uint8_t* JitBuffer;
};

extern const AsmCmdDescr AsmCmdDescrs[];

enum class AsmByteType
{
	// Данный байт не используется.
	Null,
	// 8-ь бит указаны.
	Code,
	// 7-ь бит указаны, 8-ой бит выбирается автоматически.
	CodeW,
	// 2-а бита указаны, 3 и 3 бита выбираются автоматически.
	CodeRegReg,
	// Все биты выбираются автоматически.
	ModeRegRm,
	// 5-ь бит указаны, 3 бита выбираются автоматически.
	CodeReg,
	// 2 бита выбираются автоматически, 3 бита указаны, 3 бита выбираются автоматически.
	ModeCodeRm,
	// 1 байт, константа.
	Imm8,
	// 2 байта, константа.
	Imm16,
	// 4 байта, константа.
	Imm32,
};

struct AsmCmdCode
{
	// Тип команды. Мнемоническое название.
	AsmCmdType Type;
	// Тип 1 аргумента.
	AsmArgType Arg1Type;
	// Тип 2 аргумента.
	AsmArgType Arg2Type;

	// Массив байт, кодирующих префиксы.
	uint8_t Prefixes[4];
	// [0-4] Количество записанных в Prefixes байт.
	uint8_t PrefixesSize;

	// Массив байт, кодирующих команду.
	// Opcode[1|2] + ModeR/M[0|1] + SIB[0|1]
	uint8_t Code[4];
	// [1|2] Размер Opcode.
	uint8_t OpcodeSize;
	// [0|1] Есть ли ModeR/M байт в команде.
	uint8_t ModeRmSize;
	// [0|1] Есть ли SIB байт в команде.
	uint8_t SibSize;

	// [0|1|2|4] Размер непосредственного значения в команде.
	uint8_t ImmSize;

	// [Code|CodeW] Тип последнего байта Opcode.
	AsmByteType OpcodeLastByteType;
	// [CodeRegReg|ModeRegRm|CodeReg|ModeCodeRm] Тип байта ModeR/M.
	AsmByteType ModeRmType;
};

struct AsmListingType
{
	int Commands : 1;
	int Codes    : 1;
};

union AsmModRmByte
{
	uint8_t Value;

	struct
	{
		uint8_t reg2 : 3;
		uint8_t reg1 : 3;
		uint8_t code : 2;
	} CodeRegReg;

	struct
	{
		uint8_t rm   : 3;
		uint8_t reg  : 3;
		uint8_t mode : 2;
	} ModeRegRm;

	struct
	{
		uint8_t rm   : 3;
		uint8_t code : 3;
		uint8_t mode : 2;
	} ModeCodeRm;


	struct
	{
		uint8_t reg  : 3;
		uint8_t code : 5;
	} CodeReg;

	struct
	{
		uint8_t cond : 4;
		uint8_t code : 4;
	} CondJmp;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

const size_t AsmRepLabelsDefaultCapacity   = 4096;
const size_t AsmRepCommandsDefaultCapacity = 4096;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AsmRepConstructor(AsmRep* const asmRep);

void AsmRepDestructor(AsmRep* const asmRep);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AsmCreateCommand(AsmRep* const asmRep, const AsmCmdType type, const AsmArg* const arg1 = nullptr, const AsmArg* const arg2 = nullptr);

inline AsmArg* AsmCreateArgImm(AsmArg* const arg, const int imm)
{
	assert(arg);

	arg->Type = AsmArgType::Imm;
	arg->Imm  = imm;

	return arg;
}

inline AsmArg* AsmCreateArgMem(AsmArg* const arg, const AsmRegType reg, const int imm)
{
	assert(arg);

	arg->Type = AsmArgType::Mem;
	arg->Imm  = imm;
	arg->Reg  = reg;

	return arg;
}

inline AsmArg* AsmCreateArgReg(AsmArg* const arg, const AsmRegType reg)
{
	assert(arg);

	arg->Type = AsmArgType::Reg;
	arg->Reg  = reg;

	return arg;
}

inline AsmArg* AsmCreateArgLabel(AsmArg* const arg, const char* const label)
{
	assert(arg);
	assert(label);

	arg->Type  = AsmArgType::Label;
	arg->Label = label;

	return arg;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

const char* AsmGetCommandName(const AsmCmdType type);

const char* AsmGetRegName(const AsmRegType type);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus LabelTableConstructor(AsmLabelTable* const table);

void LabelTableDestructor(AsmLabelTable* const table);

ProgramStatus LabelTableAddLabel(AsmLabelTable* const table, const char* const labelName, const size_t offset);

ProgramStatus LabelTableAddInsert(AsmLabelTable* const table, const char* const labelName, int* const address);

ProgramStatus LabelTableWrite(const AsmLabelTable* const table);

ProgramStatus LabelTabelApplyOffset(const AsmLabelTable* const table, const size_t offset);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !ASM_REPRESENTATION_H