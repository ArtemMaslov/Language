#ifndef ASM_REPRESENTATION_H
#define ASM_REPRESENTATION_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#include <stdint.h>
#include "../../Modules/ExtArray/ExtArray.h"
#include "../../Modules/ExtHeap/ExtHeap.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

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
	int         Imm;
	const char* Label;
};

struct AsmCode
{
	// Бинарное представление команды. Размер массива вмещает самую длинную команду.
	// Opcode[1|2] + MemRM[0|1] + SIB[0|1] + Displacement[0|1|2|4] + Immediate[0|1|2|4].
	uint8_t Code[2 + 1 + 1 + 4 + 4];
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

struct AsmListingType
{
	int Commands : 1;
	int Codes    : 1;
};

union AsmCodeFirstByte
{
	uint8_t Int;

	struct
	{
		uint8_t w    : 1;
		uint8_t code : 7;
	} FieldW;

	struct
	{
		uint8_t w    : 1;
		uint8_t s    : 1;
		uint8_t code : 6;
	} FieldSW;
};

union AsmCodeSecondByte
{
	uint8_t Int;

	struct
	{
		uint8_t reg2 : 3;
		uint8_t reg1 : 3;
		uint8_t code : 2;
	} RegReg;

	struct
	{
		uint8_t rm  : 3;
		uint8_t reg : 3;
		uint8_t mod : 2;
	} ModRM;

	struct
	{
		uint8_t reg  : 3;
		uint8_t code : 5;
	} Reg;

	struct
	{
		uint8_t cond : 4;
		uint8_t code : 4;
	} CondJmp;
};

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const size_t AsmRepLabelsDefaultCapacity   = 4096;
const size_t AsmRepCommandsDefaultCapacity = 4096;

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus AsmRepConstructor(AsmRep* const asmRep);

void AsmRepDestructor(AsmRep* const asmRep);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

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

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const char* AsmGetCommandName(const AsmCmdType type);

const char* AsmGetRegName(const AsmRegType type);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus LabelTableConstructor(AsmLabelTable* const table);

void LabelTableDestructor(AsmLabelTable* const table);

ProgramStatus LabelTableAddLabel(AsmLabelTable* const table, const char* const labelName, const size_t offset);

ProgramStatus LabelTableAddInsert(AsmLabelTable* const table, const char* const labelName, int* const address);

ProgramStatus LabelTableWrite(const AsmLabelTable* const table);

ProgramStatus LabelTabelApplyOffset(const AsmLabelTable* const table, const size_t offset);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !ASM_REPRESENTATION_H