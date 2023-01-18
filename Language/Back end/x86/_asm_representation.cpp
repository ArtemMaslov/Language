#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <Windows.h>
#include <memoryapi.h>

#include "x86Compiler.h"

#include "DSL.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define CHECK_STATUS \
	assert(status == ProgramStatus::Ok)

#define ASM_CMD(cmd, name) name,
const char* const AsmCommandNames[] =
{
#include "_asm_cmd_names.inc"
};
#undef ASM_CMD

#define ASM_REG(reg) #reg,
const char* const AsmRegNames[] =
{
#include "_asm_regs.inc"
};
#undef ASM_REG

#define ASM_CMD(type, arg1, arg2, code, codeSize) \
	.Type = AsmCmdType::type, \
	.Arg1 = AsmArgType::arg1, \
	.Arg2 = AsmArgType::arg2, \
	.Code = {.Code = {code}, .CodeSize = codeSize}},
const AsmCmdDescr AsmCmdDescrs[] = 
{
	{}
//#include "_asm_cmd_descr.inc"
};
#undef ASM_CMD

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static void AsmWriteArg(const AsmArg* const arg, char* const buffer, const size_t bufferSize);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus AsmRepConstructor(AsmRep* const asmRep)
{
	assert(asmRep);

	ProgramStatus status = ProgramStatus::Ok;

	asmRep->CurCmdOffset = 0;

	status = ExtArrayConstructor(&asmRep->Commands, sizeof(AsmCmd), AsmRepCommandsDefaultCapacity);
	CHECK_STATUS;

	status = ExtHeapConstructor(&asmRep->Labels, AsmLabelsDefaultCapacity);
	CHECK_STATUS;

	status = LabelTableConstructor(&asmRep->LabelTable);
	CHECK_STATUS;

	return status;
}

void AsmRepDestructor(AsmRep* const asmRep)
{
	assert(asmRep);

	ExtArrayDestructor(&asmRep->Commands);
	ExtHeapDestructor(&asmRep->Labels);
	LabelTableDestructor(&asmRep->LabelTable);

	VirtualFree(asmRep->JitBuffer, 0, MEM_RELEASE);
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus AsmCreateCommand(AsmRep* const asmRep, const AsmCmdType type, const AsmArg* const arg1, const AsmArg* const arg2)
{
	assert(asmRep);
	//assert(arg1);
	//assert(arg2);

	AsmCmd cmd = {};
	cmd.Type = type;

	if (arg1)
		memcpy(&cmd.Arg1, arg1, sizeof(AsmArg));
	else
		cmd.Arg1.Type = AsmArgType::Null;

	if (arg2)
		memcpy(&cmd.Arg2, arg2, sizeof(AsmArg));
	else
		cmd.Arg2.Type = AsmArgType::Null;

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayAddElem(&asmRep->Commands, &cmd);
	CHECK_STATUS;
	
	return status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const int    AsmListingAddressSize   = 7;
const size_t AsmListingCommandsStart = 40;

static void ToBin(uint8_t num, char** const buffer)
{
	assert(buffer);
	assert(*buffer);

	char* const buf = *buffer;

	int st = 7;
	while (st >= 0)
	{
		if (num % 2 > 0)
			buf[st--] = '1';
		else
			buf[st--] = '0';
		num /= 2;
	}

	*buffer += 8;
}

static void ToHex(uint8_t num, char** const buffer)
{
	assert(buffer);
	assert(*buffer);

	const char* const hexConvert = "0123456789abcdef";
	char* const buf = *buffer;

	int st = 1;
	while (st >= 0)
	{
		buf[st--] = hexConvert[num% 16];
		num /= 16;
	}

	*buffer += 2;
}

ProgramStatus x86WriteListingFile(const x86Compiler* const comp, const char* const fileName, AsmListingType type)
{
	assert(comp);
	assert(fileName);

	const AsmRep* const asmRep = &comp->AsmRep;

	ProgramStatus status = ProgramStatus::Ok;

	FILE* file = fopen(fileName, "w");
	if (!file)
	{
		return ProgramStatus::Fault;
	}

	const ExtArray* const commands = &asmRep->Commands;
	const size_t commandsCount = commands->Size;

	size_t cmdAddr = 0;

	for (size_t st = 0; st < commandsCount; st++)
	{
		AsmCmd* asmCmd  = (AsmCmd*)ExtArrayGetElemAt(commands, st);

		if (asmCmd->Type == AsmCmdType::label)
		{
			fprintf(file, "%s:\n",  asmCmd->Arg1.Label);
		}
		else
		{
			const size_t bufHexSize    = 30;
			char  bufHex[bufHexSize]   = "";
			char* bufHexPtr            = bufHex;

			const size_t bufCodeSize   = 256;
			char  bufCode[bufCodeSize] = "";
			char* bufCodePtr           = bufCode;

			const size_t bufCmdSize    = 32;
			char  bufCmd[bufCmdSize]   = "";

			if (type.Codes)
			{
				const size_t byteCount = asmCmd->Code.CodeSize;

				assert(byteCount * 9 + 1 < bufCodeSize);
				assert(byteCount * 3 + 1 < bufHexSize);

				for (size_t indexByte = 0; indexByte < byteCount; indexByte++)
				{
					ToBin(asmCmd->Code.Code[indexByte], &bufCodePtr);
					ToHex(asmCmd->Code.Code[indexByte], &bufHexPtr);

					*(bufCodePtr++) = ' ';
					*(bufHexPtr++)  = ' ';
				}
			}

			if (type.Commands)
			{
				const char* cmdName = AsmGetCommandName(asmCmd->Type);

				if (asmCmd->Arg1.Type != AsmArgType::Null && asmCmd->Arg2.Type != AsmArgType::Null)
				{
					const size_t argBufferSize = 32;
					char  arg1[argBufferSize]  = "";
					char  arg2[argBufferSize]  = "";

					AsmWriteArg(&asmCmd->Arg1, arg1, argBufferSize);
					AsmWriteArg(&asmCmd->Arg2, arg2, argBufferSize);

					snprintf(bufCmd, bufCmdSize, "%s %s, %s", cmdName, arg1, arg2);
				}
				else if (asmCmd->Arg1.Type != AsmArgType::Null)
				{
					const size_t argBufferSize = 32;
					char  arg[argBufferSize]  = "";

					AsmWriteArg(&asmCmd->Arg1, arg, argBufferSize);

					snprintf(bufCmd, bufCmdSize, "%s %s", cmdName, arg);
				}
				else
				{
					strncpy(bufCmd, cmdName, bufCmdSize);
				}
			}

			if (type.Commands && type.Codes)
			{
				fprintf(file, "0x%0*zx: %-*s %-*s %s\n",
						AsmListingAddressSize, cmdAddr, 
						(int)bufCmdSize, bufCmd,
						(int)bufHexSize, bufHex,
						bufCode);
			}
			else if (type.Commands)
			{
				fprintf(file, "\t%s\n", bufCmd);
			}
		}

		cmdAddr += asmCmd->Code.CodeSize;
	}

	fclose(file);

	return status;
}

static void AsmWriteArg(const AsmArg* const arg, char* const buffer, const size_t bufferSize)
{
	assert(arg);
	assert(buffer);

	int imm        = arg->Imm;
	AsmRegType reg = arg->Reg;

	switch (arg->Type)
	{
		case AsmArgType::Imm:
			snprintf(buffer, bufferSize, "%d", imm);
			break;

		case AsmArgType::Mem:
			if (imm != 0 && reg != AsmRegType::null)
			{
				snprintf(buffer, bufferSize, "[%s + %d]", AsmGetRegName(reg), imm);
			}
			else if (reg != AsmRegType::null)
			{
				snprintf(buffer, bufferSize, "[%s]", AsmGetRegName(reg));
			}
			else
			{
				snprintf(buffer, bufferSize, "[%d]", imm);
			}
			break;

		case AsmArgType::Reg:
			snprintf(buffer, bufferSize, "%s", AsmGetRegName(reg));
			break;

		case AsmArgType::Label:
			snprintf(buffer, bufferSize, "%s", arg->Label);
			break;

		default:
			assert(!"Error");
			break;
	}
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const char* AsmGetCommandName(const AsmCmdType type)
{
	switch (type)
	{
		case AsmCmdType::label:
			assert(!"Error");
			return nullptr;

		case AsmCmdType::null:
			return "(null)";

		default:
			return AsmCommandNames[(int)type];
	}
}

const char* AsmGetRegName(const AsmRegType type)
{
	switch (type)
	{
		case AsmRegType::null:
			assert(!"Error");
			return nullptr;

		default:
			return AsmRegNames[(int)type];
	}
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static uint8_t AsmGetRegEnc32W(const AsmRegType reg, uint8_t* out_w);
static uint8_t AsmGetRegEnc32(const AsmRegType reg);
static ProgramStatus AsmTranslateAddSub(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateMulDiv(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslatePush(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslatePop(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateMov(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateXor(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateNeg(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateDecInc(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateCmp(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateCondJmp(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateJmpCall(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateRet(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateCvtsi2sd(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateMovsd(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateCdq(AsmRep* const asmRep, AsmCmd* const cmd);
static uint8_t AsmGetPopReg(const AsmRegType reg);

static ProgramStatus AsmTranslateFchs(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateFaddpFsubp(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateFmulpFdivp(AsmRep* const asmRep, AsmCmd* const cmd);

static ProgramStatus AsmTranslateFld(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateFstp(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateFcompp(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateFrnd(AsmRep* const asmRep, AsmCmd* const cmd);
static ProgramStatus AsmTranslateFistp(AsmRep* const asmRep, AsmCmd* const cmd);

ProgramStatus AsmTranslateCommands(x86Compiler* const comp)
{
	assert(comp);

	ProgramStatus status = ProgramStatus::Ok;

	const ExtArray* const commands = &comp->AsmRep.Commands;
	const size_t cmdCount = comp->AsmRep.Commands.Size;

	for (size_t st = 0; st < cmdCount; st++)
	{
		AsmCmd* cmd = (AsmCmd*)ExtArrayGetElemAt(commands, st);

		switch (cmd->Type)
		{
			case AsmCmdType::label:
				status = LabelTableAddLabel(&comp->AsmRep.LabelTable, cmd->Arg1.Label, comp->AsmRep.CurCmdOffset);
				CHECK_STATUS;
				break;

			case AsmCmdType::add:
			case AsmCmdType::sub:
				status = AsmTranslateAddSub(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::faddp:
			case AsmCmdType::fsubp:
				status = AsmTranslateFaddpFsubp(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::mul:
			case AsmCmdType::imul:
			case AsmCmdType::div:
			case AsmCmdType::idiv:
				status = AsmTranslateMulDiv(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::fmulp:
			case AsmCmdType::fdivp:
				status = AsmTranslateFmulpFdivp(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::push:
				status = AsmTranslatePush(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::pop:
				status = AsmTranslatePop(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::mov:
				status = AsmTranslateMov(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::neg:
				status = AsmTranslateNeg(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::fchs:
				status = AsmTranslateFchs(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::fld:
				status = AsmTranslateFld(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::fstp:
			case AsmCmdType::fstpd:
				status = AsmTranslateFstp(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::fcompp:
				status = AsmTranslateFcompp(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::_xor:
				status = AsmTranslateXor(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::cmp:
				status = AsmTranslateCmp(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::inc:
			case AsmCmdType::dec:
				assert(!"Not implemented");
				break;

			case AsmCmdType::ja:
			case AsmCmdType::jae:
			case AsmCmdType::jb:
			case AsmCmdType::jbe:
			case AsmCmdType::je:
			case AsmCmdType::jne:
			case AsmCmdType::jg:
			case AsmCmdType::jge:
			case AsmCmdType::jl:
			case AsmCmdType::jle:
				status = AsmTranslateCondJmp(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::jmp:
			case AsmCmdType::call:
				status = AsmTranslateJmpCall(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::ret:
				status = AsmTranslateRet(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::cvtsi2sd:
				status = AsmTranslateCvtsi2sd(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::movsd:
				status = AsmTranslateMovsd(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::cdq:
				status = AsmTranslateCdq(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::frnd:
				status = AsmTranslateFrnd(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			case AsmCmdType::fistp:
				status = AsmTranslateFistp(&comp->AsmRep, cmd);
				CHECK_STATUS;
				break;

			default:
				assert(!"Error");
				break;
		}
	}

	comp->AsmRep.JitBufferSize = comp->AsmRep.CurCmdOffset;

	return status;
}

static ProgramStatus AsmTranslateAddSub(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Reg) && ARG2_TYPE(Reg))
	{
		uint8_t w1 = 0;
		uint8_t reg1 = AsmGetRegEnc32W(cmd->Arg1.Reg, &w1);

		uint8_t w2 = 0;
		uint8_t reg2 = AsmGetRegEnc32W(cmd->Arg2.Reg, &w2);

		assert(w1 == w2);

		uint8_t code = 0;
		switch (cmd->Type)
		{
			case AsmCmdType::add:
				code = 0b0000001;
				break;

			case AsmCmdType::sub:
				code = 0b0010101;
				break;

			default:
				assert(!"Error");
				break;
		}

		AsmCodeFirstByte fb  = {};
		fb.FieldW.code       = code;
		fb.FieldW.w          = w1;

		AsmCodeSecondByte sb = {};
		sb.RegReg.code       = 0b11;
		sb.RegReg.reg1       = reg1;
		sb.RegReg.reg2       = reg2;

		cmd->Code.Code[0]    = fb.Int;
		cmd->Code.Code[1]    = sb.Int;
		cmd->Code.CodeSize   = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}
	else if (ARG1_TYPE(Reg) && ARG2_TYPE(Imm))
	{
		uint8_t w = 0;
		uint8_t reg = AsmGetRegEnc32W(cmd->Arg1.Reg, &w);

		AsmCodeFirstByte fb  = {};
		fb.FieldSW.code      = 0b100000;
		fb.FieldSW.s         = 0;
		fb.FieldSW.w         = w;

		AsmCodeSecondByte sb = {};
		sb.Reg.reg           = reg;
		switch (cmd->Type)
		{
			case AsmCmdType::add:
				sb.Reg.code = 0b11000;
				break;

			case AsmCmdType::sub:
				sb.Reg.code = 0b11101;
				break;
		}

		cmd->Code.Code[0] = fb.Int;
		cmd->Code.Code[1] = sb.Int;
		memcpy(cmd->Code.Code + 2, &cmd->Arg2.Imm, 4);
		cmd->Code.CodeSize = 6;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}
	/*else if (ARG1_TYPE(Reg) && ARG2_TYPE(Mem))
	{
	}
	else if (ARG1_TYPE(Mem) && ARG2_TYPE(Reg))
	{
	}
	else if (ARG1_TYPE(Reg) && ARG2_TYPE(Imm))
	{
	}*/

	assert(!"Not implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateMulDiv(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Reg) && ARG2_TYPE(Null))
	{
		uint8_t w   = 0;
		uint8_t reg = AsmGetRegEnc32W(cmd->Arg1.Reg, &w);

		uint8_t code1 = 0;
		uint8_t code2 = 0;
		uint8_t mod   = 0;
		switch (cmd->Type)
		{
			case AsmCmdType::mul:
				code1 = 0b1111011;
				code2 = 0b11100;
				break;

			case AsmCmdType::imul:
				code1 = 0b1111011;
				code2 = 0b11101;
				break;

			case AsmCmdType::div:
				code1 = 0b1111011;
				code2 = 0b11110;
				break;

			case AsmCmdType::idiv:
				code1 = 0b1111011;
				code2 = 0b11111;
				break;

			default:
				assert(!"Error");
				break;
		}

		AsmCodeFirstByte fb  = {};
		fb.FieldW.code       = code1;
		fb.FieldW.w          = w;

		AsmCodeSecondByte sb = {};
		sb.Reg.code          = code2;
		sb.Reg.reg           = reg;

		cmd->Code.Code[0]    = fb.Int;
		cmd->Code.Code[1]    = sb.Int;
		cmd->Code.CodeSize   = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}
	/*else if (ARG1_TYPE(Mem) && ARG2_TYPE(Null))
	{

	}*/

	assert(!"Not implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslatePush(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Reg) && ARG2_TYPE(Null))
	{
		uint8_t reg = AsmGetRegEnc32(cmd->Arg1.Reg);

		AsmCodeSecondByte sb = {};
		sb.Reg.code          = 0b01010;
		sb.Reg.reg           = reg;

		cmd->Code.Code[0]  = sb.Int;
		cmd->Code.CodeSize = 1;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}
	else if (ARG1_TYPE(Imm) && ARG2_TYPE(Null))
	{
		AsmCodeFirstByte fb = {};
		fb.FieldSW.code     = 0b011010;
		fb.FieldSW.s        = 0;
		fb.FieldW.w         = 0;

		cmd->Code.Code[0]  = fb.Int;
		memcpy(cmd->Code.Code + 1, &cmd->Arg1.Imm, 4);
		cmd->Code.CodeSize = 5;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}
	else if (ARG1_TYPE(Mem) && ARG2_TYPE(Null))
	{
		uint8_t fb = 0b11111111;

		uint8_t rm = AsmGetPopReg(cmd->Arg1.Reg);

		AsmCodeSecondByte sb = {};
		sb.ModRM.mod         = 0b10;
		sb.ModRM.reg         = 0b110;
		sb.ModRM.rm          = rm;

		cmd->Code.Code[0]    = fb;
		cmd->Code.Code[1]    = sb.Int;

		memcpy(cmd->Code.Code + 2, &cmd->Arg1.Imm, 4);

		cmd->Code.CodeSize = 6;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslatePop(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Reg) && ARG2_TYPE(Null))
	{
		uint8_t reg = AsmGetRegEnc32(cmd->Arg1.Reg);

		AsmCodeSecondByte sb = {};
		sb.Reg.code          = 0b01011;
		sb.Reg.reg           = reg;

		cmd->Code.Code[0]  = sb.Int;
		cmd->Code.CodeSize = 1;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}
	else if (ARG1_TYPE(Mem) && ARG2_TYPE(Null))
	{
		AsmCodeFirstByte fb  = {};
		fb.Int               = 0b10001111;

		uint8_t rm = AsmGetPopReg(cmd->Arg1.Reg);

		AsmCodeSecondByte sb = {};
		sb.ModRM.mod         = 0b10;
		sb.ModRM.reg         = 0b000;
		sb.ModRM.rm          = rm;

		cmd->Code.Code[0]  = fb.Int;
		cmd->Code.Code[1]  = sb.Int;
		
		memcpy(cmd->Code.Code + 2, &cmd->Arg1.Imm, 4);

		cmd->Code.CodeSize = 6;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateMov(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Reg) && ARG2_TYPE(Reg))
	{
		uint8_t w1 = 0;
		uint8_t reg1 = AsmGetRegEnc32W(cmd->Arg1.Reg, &w1);

		uint8_t w2 = 0;
		uint8_t reg2 = AsmGetRegEnc32W(cmd->Arg2.Reg, &w2);

		assert(w1 == w2);

		AsmCodeFirstByte fb = {};
		fb.FieldW.code      = 0b1000101;
		fb.FieldW.w         = w1;

		AsmCodeSecondByte sb = {};
		sb.RegReg.code       = 0b11;
		sb.RegReg.reg1       = reg1;
		sb.RegReg.reg2       = reg2;

		cmd->Code.Code[0]    = fb.Int;
		cmd->Code.Code[1]    = sb.Int;
		cmd->Code.CodeSize   = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}
	else if (ARG1_TYPE(Reg) && ARG2_TYPE(Imm))
	{
		uint8_t w   = 0;
		uint8_t reg = AsmGetRegEnc32W(cmd->Arg1.Reg, &w);

		AsmCodeFirstByte fb  = {};
		fb.FieldW.code       = 0b1100011;
		fb.FieldW.w          = w;

		AsmCodeSecondByte sb = {};
		sb.Reg.code          = 0b11000;
		sb.Reg.reg           = reg;

		cmd->Code.Code[0]    = fb.Int;
		cmd->Code.Code[1]    = sb.Int;
		memcpy(cmd->Code.Code + 2, &cmd->Arg2.Imm, 4);
		cmd->Code.CodeSize   = 6;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateXor(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Reg) && ARG2_TYPE(Reg))
	{
		uint8_t w1   = 0;
		uint8_t reg1 = AsmGetRegEnc32W(cmd->Arg1.Reg, &w1);

		uint8_t w2   = 0;
		uint8_t reg2 = AsmGetRegEnc32W(cmd->Arg2.Reg, &w2);

		assert(w1 == w2);

		AsmCodeFirstByte fb  = {};
		fb.FieldW.code       = 0b0011001;
		fb.FieldW.w          = w1;

		AsmCodeSecondByte sb = {};
		sb.RegReg.code       = 0b11;
		sb.RegReg.reg1       = reg1;
		sb.RegReg.reg2       = reg2;

		cmd->Code.Code[0]  = fb.Int;
		cmd->Code.Code[1]  = sb.Int;
		cmd->Code.CodeSize = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateNeg(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Reg) && ARG2_TYPE(Null))
	{
		uint8_t w   = 0;
		uint8_t reg = AsmGetRegEnc32W(cmd->Arg1.Reg, &w);

		AsmCodeFirstByte fb  = {};
		fb.FieldW.code       = 0b1111011;
		fb.FieldW.w          = w;

		AsmCodeSecondByte sb = {};
		sb.Reg.code          = 0b11011;
		sb.Reg.reg           = reg;

		cmd->Code.Code[0]  = fb.Int;
		cmd->Code.Code[1]  = sb.Int;
		cmd->Code.CodeSize = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}
	
	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateDecInc(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateCmp(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Reg) && ARG2_TYPE(Reg))
	{
		uint8_t w1   = 0;
		uint8_t reg1 = AsmGetRegEnc32W(cmd->Arg1.Reg, &w1);

		uint8_t w2   = 0;
		uint8_t reg2 = AsmGetRegEnc32W(cmd->Arg2.Reg, &w2);

		assert(w1 == w2);

		AsmCodeFirstByte fb  = {};
		fb.FieldW.code       = 0b0011101;
		fb.FieldW.w          = w1;

		AsmCodeSecondByte sb = {};
		sb.RegReg.code       = 0b11;
		sb.RegReg.reg1       = reg1;
		sb.RegReg.reg2       = reg2;

		cmd->Code.Code[0]    = fb.Int;
		cmd->Code.Code[1]    = sb.Int;
		cmd->Code.CodeSize   = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}
	else if (ARG1_TYPE(Imm) && ARG2_TYPE(Reg))
	{
		uint8_t w   = 0;
		uint8_t reg = AsmGetRegEnc32W(cmd->Arg2.Reg, &w);

		AsmCodeFirstByte fb  = {};
		fb.FieldSW.code      = 0b100000;
		fb.FieldSW.s         = 0;
		fb.FieldSW.w         = w;

		AsmCodeSecondByte sb = {};
		sb.Reg.code          = 0b11111;
		sb.Reg.reg           = reg;

		cmd->Code.Code[0]    = fb.Int;
		cmd->Code.Code[1]    = sb.Int;
		memcpy(cmd->Code.Code + 2, &cmd->Arg1.Imm, 4);
		cmd->Code.CodeSize   = 6;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateCondJmp(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (!(ARG1_TYPE(Label) && ARG2_TYPE(Null)))
	{
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	uint8_t fb = 0b00001111;

	AsmCodeSecondByte sb = {};
	sb.CondJmp.code      = 0b1000;

	switch (cmd->Type)
	{
		case AsmCmdType::ja:
			sb.CondJmp.cond = 0b0111;
			break;
		case AsmCmdType::jae:
			sb.CondJmp.cond = 0b0011;
			break;
		case AsmCmdType::jb:
			sb.CondJmp.cond = 0b0010;
			break;
		case AsmCmdType::jbe:
			sb.CondJmp.cond = 0b0110;
			break;
		case AsmCmdType::je:
			sb.CondJmp.cond = 0b0100;
			break;
		case AsmCmdType::jne:
			sb.CondJmp.cond = 0b0101;
			break;
		case AsmCmdType::jg:
			sb.CondJmp.cond = 0b1111;
			break;
		case AsmCmdType::jge:
			sb.CondJmp.cond = 0b1101;
			break;
		case AsmCmdType::jl:
			sb.CondJmp.cond = 0b1100;
			break;
		case AsmCmdType::jle:
			sb.CondJmp.cond = 0b1110;
			break;
	}

	cmd->Code.Code[0]  = fb;
	cmd->Code.Code[1]  = sb.Int;
	cmd->Code.CodeSize = 6;

	asmRep->CurCmdOffset += cmd->Code.CodeSize;

	memcpy(cmd->Code.Code + 2, &asmRep->CurCmdOffset, 4);

	LabelTableAddInsert(&asmRep->LabelTable, cmd->Arg1.Label, (int*)(cmd->Code.Code + 2));

	return ProgramStatus::Ok;
}

static ProgramStatus AsmTranslateJmpCall(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (!(ARG1_TYPE(Label) && ARG2_TYPE(Null)))
	{
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	uint8_t fb = 0;
	switch (cmd->Type)
	{
		case AsmCmdType::call:
			fb = 0b11101000;
			break;
		case AsmCmdType::jmp:
			fb = 0b11101001;
			break;
	}

	cmd->Code.Code[0]  = fb;
	cmd->Code.CodeSize = 5;

	asmRep->CurCmdOffset += cmd->Code.CodeSize;

	memcpy(cmd->Code.Code + 1, &asmRep->CurCmdOffset, 4);

	LabelTableAddInsert(&asmRep->LabelTable, cmd->Arg1.Label, (int*)(cmd->Code.Code + 1));

	return ProgramStatus::Ok;
}

static ProgramStatus AsmTranslateRet(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (!(ARG1_TYPE(Null) && ARG2_TYPE(Null)))
	{
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	cmd->Code.Code[0]  = 0b11000011;
	cmd->Code.CodeSize = 1;

	asmRep->CurCmdOffset += cmd->Code.CodeSize;

	return ProgramStatus::Ok;
}

static ProgramStatus AsmTranslateCvtsi2sd(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Reg) && ARG2_TYPE(Reg))
	{
		uint8_t xmmReg = AsmGetRegEnc32(cmd->Arg1.Reg);
		uint8_t genReg = AsmGetRegEnc32(cmd->Arg2.Reg);

		uint8_t b1 = 0b11110011;
		uint8_t b2 = 0b00001111;
		uint8_t b3 = 0b00101010;

		AsmCodeSecondByte b4 = {};
		b4.ModRM.mod         = 0b11;
		b4.ModRM.reg         = xmmReg;
		b4.ModRM.rm          = genReg;

		cmd->Code.Code[0]    = b1;
		cmd->Code.Code[1]    = b2;
		cmd->Code.Code[2]    = b3;
		cmd->Code.Code[3]    = b4.Int;
		cmd->Code.CodeSize   = 4;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateMovsd(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Mem) && ARG2_TYPE(Reg))
	{
		uint8_t xmmReg = AsmGetRegEnc32(cmd->Arg2.Reg);
		uint8_t genReg = AsmGetRegEnc32(cmd->Arg1.Reg);

		uint8_t b1 = 0b11110011;
		uint8_t b2 = 0b00001111;
		uint8_t b3 = 0b00010001;

		AsmCodeSecondByte b4 = {};
		b4.ModRM.mod         = 0b10;
		b4.ModRM.reg         = xmmReg;
		b4.ModRM.rm          = genReg;

		cmd->Code.Code[0]    = b1;
		cmd->Code.Code[1]    = b2;
		cmd->Code.Code[2]    = b3;
		cmd->Code.Code[3]    = b4.Int;
		memcpy(cmd->Code.Code + 4, &cmd->Arg2.Imm, 4);
		cmd->Code.CodeSize   = 8;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateCdq(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (!(ARG1_TYPE(Null) && ARG2_TYPE(Null)))
	{
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	cmd->Code.Code[0]  = 0b10011001;
	cmd->Code.CodeSize = 1;

	asmRep->CurCmdOffset += cmd->Code.CodeSize;

	return ProgramStatus::Ok;
}

static uint8_t AsmGetPopReg(const AsmRegType reg)
{
	switch (reg)
	{
		case AsmRegType::eax:
			return 0b000;
		case AsmRegType::ebx:
			return 0b011;
		case AsmRegType::ecx:
			return 0b001;
		case AsmRegType::edx:
			return 0b010;
		case AsmRegType::null:
			return 0b100;
		case AsmRegType::ebp:
			return 0b101;
		case AsmRegType::esi:
			return 0b110;
		case AsmRegType::edi:
			return 0b111;
		default:
			assert(!"Error");
			return 0;
	}
}

static uint8_t AsmGetRegEnc32W(const AsmRegType reg, uint8_t* out_w)
{
	assert(out_w);

	switch (reg)
	{
		case AsmRegType::al:
			*out_w = 0;
			return 0b000;
		case AsmRegType::bl:
			*out_w = 0;
			return 0b011;
		case AsmRegType::cl:
			*out_w = 0;
			return 0b001;
		case AsmRegType::dl:
			*out_w = 0;
			return 0b010;
		case AsmRegType::ah:
			*out_w = 0;
			return 0b100;
		case AsmRegType::bh:
			*out_w = 0;
			return 0b111;
		case AsmRegType::ch:
			*out_w = 0;
			return 0b101;
		case AsmRegType::dh:
			*out_w = 0;
			return 0b110;

		case AsmRegType::eax:
			*out_w = 1;
			return 0b000;
		case AsmRegType::ebx:
			*out_w = 1;
			return 0b011;
		case AsmRegType::ecx:
			*out_w = 1;
			return 0b001;
		case AsmRegType::edx:
			*out_w = 1;
			return 0b010;
		case AsmRegType::esp:
			*out_w = 1;
			return 0b100;
		case AsmRegType::ebp:
			*out_w = 1;
			return 0b101;
		case AsmRegType::esi:
			*out_w = 1;
			return 0b110;
		case AsmRegType::edi:
			*out_w = 1;
			return 0b111;

		default:
			assert(!"Error");
			return 0;
	}
}

static uint8_t AsmGetRegEnc32(const AsmRegType reg)
{
	switch (reg)
	{
		case AsmRegType::xmm0:
		case AsmRegType::eax:
			return 0b000;

		case AsmRegType::xmm1:
		case AsmRegType::ebx:
			return 0b011;

		case AsmRegType::xmm2:
		case AsmRegType::ecx:
			return 0b001;

		case AsmRegType::xmm3:
		case AsmRegType::edx:
			return 0b010;

		case AsmRegType::xmm4:
		case AsmRegType::esp:
			return 0b100;

		case AsmRegType::xmm5:
		case AsmRegType::ebp:
			return 0b101;

		case AsmRegType::xmm6:
		case AsmRegType::esi:
			return 0b110;

		case AsmRegType::xmm7:
		case AsmRegType::edi:
			return 0b111;

		default:
			assert(!"Error");
			return 0;
	}
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus LabelTableConstructor(AsmLabelTable* const table)
{
	assert(table);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayConstructor(&table->Labels, sizeof(AsmLabelNode), AsmLabelsDefaultCapacity);
	CHECK_STATUS;

	status = ExtArrayConstructor(&table->Inserts, sizeof(AsmLabelInsert), AsmInsertsDefaultCapacity);
	CHECK_STATUS;

	return status;
}

ProgramStatus LabelTableAddLabel(AsmLabelTable* const table, const char* const labelName, const size_t offset)
{
	assert(table);
	assert(labelName);

	AsmLabelNode label   = {};
	label.LabelName      = labelName;
	label.RelativeOffset = offset;

	return ExtArrayAddElem(&table->Labels, &label);
}

ProgramStatus LabelTableAddInsert(AsmLabelTable* const table, const char* const labelName, int* const address)
{
	assert(table);
	assert(labelName);
	assert(address);

	AsmLabelInsert insert = {};
	insert.Insert         = address;
	insert.LabelName      = labelName;

	return ExtArrayAddElem(&table->Inserts, &insert);
}

ProgramStatus LabelTabelApplyOffset(const AsmLabelTable* const table, const size_t offset)
{
	assert(table);

	const ExtArray* const labels  = &table->Labels;
	const size_t labelsCount = table->Labels.Size;

	for (size_t st = 0; st < labelsCount; st++)
	{
		AsmLabelNode* label = (AsmLabelNode*)ExtArrayGetElemAt(labels, st);

		label->RelativeOffset += offset;
	}

	return ProgramStatus::Ok;
}

ProgramStatus LabelTableWrite(const AsmLabelTable* const table)
{
	assert(table);

	const ExtArray* const labels  = &table->Labels;
	const ExtArray* const inserts = &table->Inserts;

	const size_t insertCount = table->Inserts.Size;
	const size_t labelsCount = table->Labels.Size;

	for (size_t st = 0; st < insertCount; st++)
	{
		AsmLabelInsert* insert = (AsmLabelInsert*)ExtArrayGetElemAt(inserts, st);

		for (size_t st1 = 0; st1 < labelsCount; st1++)
		{
			AsmLabelNode* label = (AsmLabelNode*)ExtArrayGetElemAt(labels, st1);

			if (strcmp(label->LabelName, insert->LabelName) == 0)
			{
				*insert->Insert = label->RelativeOffset - (*insert->Insert);
				break;
			}
		}
	}

	return ProgramStatus::Ok;
}

void LabelTableDestructor(AsmLabelTable* const table)
{
	assert(table);

	ExtArrayDestructor(&table->Labels);
	ExtArrayDestructor(&table->Inserts);
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ProgramStatus AsmTranslateFaddpFsubp(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Null) && ARG2_TYPE(Null))
	{
		uint8_t fb = 0b11011110;
		uint8_t sb = 0b11000000;

		switch (cmd->Type)
		{
			case AsmCmdType::faddp:
				sb = 0b11000001;
				break;

			case AsmCmdType::fsubp:
				sb = 0b11101001;
				break;

			default:
				assert(!"Error");
				break;
		}

		cmd->Code.Code[0]  = fb;
		cmd->Code.Code[1]  = sb;
		cmd->Code.CodeSize = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateFmulpFdivp(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Null) && ARG2_TYPE(Null))
	{
		uint8_t fb = 0b11011110;
		uint8_t sb = 0b11001001;

		switch (cmd->Type)
		{
			case AsmCmdType::fmulp:
				sb = 0b11001001;
				break;

			case AsmCmdType::fdivp:
				sb = 0b11111001;
				break;

			default:
				assert(!"Error");
				break;
		}

		cmd->Code.Code[0]  = fb;
		cmd->Code.Code[1]  = sb;
		cmd->Code.CodeSize = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateFchs(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Null) && ARG2_TYPE(Null))
	{
		uint8_t fb = 0b11011001;
		uint8_t sb = 0b11100000;

		cmd->Code.Code[0]  = fb;
		cmd->Code.Code[1]  = sb;
		cmd->Code.CodeSize = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateFld(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Mem) && ARG2_TYPE(Null))
	{
		uint8_t fb = 0b11011001;

		uint8_t rm = AsmGetPopReg(cmd->Arg1.Reg);

		AsmCodeSecondByte sb = {};
		sb.ModRM.mod         = 0b10;
		sb.ModRM.reg         = 0b000;
		sb.ModRM.rm          = rm;

		cmd->Code.Code[0]    = fb;
		cmd->Code.Code[1]    = sb.Int;

		memcpy(cmd->Code.Code + 2, &cmd->Arg1.Imm, 4);

		cmd->Code.CodeSize   = 6;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateFstp(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Mem) && ARG2_TYPE(Null))
	{
		uint8_t fb = 0b11011001;

		if (cmd->Type == AsmCmdType::fstpd)
			fb = 0b11011101;

		uint8_t rm = AsmGetPopReg(cmd->Arg1.Reg);

		AsmCodeSecondByte sb = {};
		sb.ModRM.mod         = 0b10;
		sb.ModRM.reg         = 0b011;
		sb.ModRM.rm          = rm;

		cmd->Code.Code[0]    = fb;
		cmd->Code.Code[1]    = sb.Int;

		memcpy(cmd->Code.Code + 2, &cmd->Arg1.Imm, 4);

		cmd->Code.CodeSize   = 6;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateFistp(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Mem) && ARG2_TYPE(Null))
	{
		uint8_t fb = 0b11011011;

		uint8_t rm = AsmGetPopReg(cmd->Arg1.Reg);

		AsmCodeSecondByte sb = {};
		sb.ModRM.mod         = 0b10;
		sb.ModRM.reg         = 0b011;
		sb.ModRM.rm          = rm;

		cmd->Code.Code[0]    = fb;
		cmd->Code.Code[1]    = sb.Int;

		memcpy(cmd->Code.Code + 2, &cmd->Arg1.Imm, 4);

		cmd->Code.CodeSize   = 6;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateFcompp(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Null) && ARG2_TYPE(Null))
	{
		// fcompp
		cmd->Code.Code[0]     = 0b11011110;
		cmd->Code.Code[1]     = 0b11011001;
		// fstsw ax
		cmd->Code.Code[2]     = 0b11011111;
		cmd->Code.Code[3]     = 0b11100000;
		// sahf
		cmd->Code.Code[4]     = 0b10011110;

		cmd->Code.CodeSize    = 5;
		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

static ProgramStatus AsmTranslateFrnd(AsmRep* const asmRep, AsmCmd* const cmd)
{
	assert(asmRep);
	assert(cmd);

	if (ARG1_TYPE(Null) && ARG2_TYPE(Null))
	{
		// frnd
		cmd->Code.Code[0]    = 0b11011001;
		cmd->Code.Code[1]    = 0b11111100;

		cmd->Code.CodeSize   = 2;

		asmRep->CurCmdOffset += cmd->Code.CodeSize;

		return ProgramStatus::Ok;
	}

	assert(!"Not Implemented");
	return ProgramStatus::Fault;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 