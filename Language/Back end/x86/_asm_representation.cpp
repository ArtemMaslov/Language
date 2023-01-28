#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <Windows.h>
#include <memoryapi.h>

#include "x86Compiler.h"

#include "DSL.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define CHECK_STATUS \
	assert(status == ProgramStatus::Ok)

#define ASM_CMD(cmd, name, ...) name,
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

#define BYTE1(b1) {b1}
#define BYTE2(b1, b2) {b1, b2}
#define BYTE3(b1, b2, b3) {b1, b2, b3}
#define BYTE4(b1, b2, b3, b4) {b1, b2, b3, b4}

#define ASM_CMD(cmd, name,								   \
				arg1, arg2,								   \
				prefixes,								   \
				prefixesSize,							   \
				code,									   \
				opcodeSize, modeRmSize, sibSize,		   \
				immSize,								   \
				opcodeLastByte,							   \
				modeRmType)								   \
	{													   \
		.Type     = AsmCmdType::cmd,					   \
		.Arg1Type = AsmArgType::arg1,					   \
		.Arg2Type = AsmArgType::arg2,					   \
														   \
		.Prefixes     = prefixes,						   \
		.PrefixesSize = prefixesSize,					   \
														   \
		.Code       = code,								   \
		.OpcodeSize = opcodeSize,						   \
		.ModeRmSize = modeRmSize,						   \
		.SibSize    = sibSize,							   \
														   \
		.ImmSize = immSize,								   \
														   \
		.OpcodeLastByteType = AsmByteType::opcodeLastByte, \
		.ModeRmType = AsmByteType::modeRmType,			   \
	},

// Массив кодировок команд.
// Содержит записи о том, как перевести команду в машинный код.
static const AsmCmdCode AsmCmdCodes[] = 
{
	{
		.Type = AsmCmdType::null
	},
#include "_asm_cmd_codes.inc"
};
// Количество записей в массиве кодировок команд.
static const size_t AsmCmdCodesCount = sizeof(AsmCmdCodes) / sizeof(AsmCmdCode);
static const size_t AsmCmdCodesNull  = 0;
#undef ASM_CMD

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static void AsmWriteArg(const AsmArg* const arg, char* const buffer, const size_t BufferSize);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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
		buf[st--] = '0' + (num % 2);
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
		buf[st--] = hexConvert[num % 16];
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

static void AsmWriteArg(const AsmArg* const arg, char* const buffer, const size_t BufferSize)
{
	assert(arg);
	assert(buffer);

	int imm        = arg->Imm;
	AsmRegType reg = arg->Reg;

	switch (arg->Type)
	{
		case AsmArgType::Imm:
			snprintf(buffer, BufferSize, "%d", imm);
			break;

		case AsmArgType::Mem:
			if (imm != 0 && reg != AsmRegType::null)
			{
				snprintf(buffer, BufferSize, "[%s + %d]", AsmGetRegName(reg), imm);
			}
			else if (reg != AsmRegType::null)
			{
				snprintf(buffer, BufferSize, "[%s]", AsmGetRegName(reg));
			}
			else
			{
				snprintf(buffer, BufferSize, "[%d]", imm);
			}
			break;

		case AsmArgType::Reg:
			snprintf(buffer, BufferSize, "%s", AsmGetRegName(reg));
			break;

		case AsmArgType::Label:
			snprintf(buffer, BufferSize, "%s", arg->Label);
			break;

		default:
			assert(!"Error");
			break;
	}
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static size_t AsmFindCmdCode(const AsmCmd* const cmd);
static uint8_t AsmGetRegEnc32W(const AsmRegType reg, uint8_t* const out_w);
static uint8_t AsmGetRegEnc32(const AsmRegType reg);
static uint8_t AsmGetModeRmByte(const AsmCmd* const cmd, const AsmCmdCode* const code, 
								   const uint8_t modRmByte, uint8_t* const out_w);
static ProgramStatus AsmCheckCmdCorrect(const AsmCmd* const cmd);
static uint8_t AsmEncodeModeRegRm(const AsmArg* const mem, const AsmArg* const reg);
static uint8_t AsmEncodeModeCodeRm(const AsmArg* const mem, const uint8_t code);

ProgramStatus AsmTranslateCommands(x86Compiler* const comp)
{
	assert(comp);

	ProgramStatus status = ProgramStatus::Ok;

	const ExtArray* const commands = &comp->AsmRep.Commands;
	const size_t cmdCount = comp->AsmRep.Commands.Size;

	for (size_t cmdIndex = 0; cmdIndex < cmdCount; cmdIndex++)
	{
		AsmCmd* cmd = (AsmCmd*)ExtArrayGetElemAt(commands, cmdIndex);

		if (cmd->Type == AsmCmdType::label)
		{
			if (LabelTableAddLabel(&comp->AsmRep.LabelTable, cmd->Arg1.Label, comp->AsmRep.CurCmdOffset) != ProgramStatus::Ok)
				return ProgramStatus::Fault;
			continue;
		}

		size_t  cmdCodeIndex = AsmFindCmdCode(cmd);

		if (cmdCodeIndex == AsmCmdCodesNull)
		{
			assert(!"Error");
			return ProgramStatus::Fault;
		}
		
		if (AsmCheckCmdCorrect(cmd) != ProgramStatus::Ok)
			return ProgramStatus::Fault;

		const AsmCmdCode* const cmdCode = AsmCmdCodes + cmdCodeIndex;
		size_t codeOutputIndex = 0;

		const size_t prefixesSize = cmdCode->PrefixesSize;
		assert(prefixesSize <= 4);
		for (size_t st = 0; st < prefixesSize; st++)
			cmd->Code.Code[codeOutputIndex++] = cmdCode->Prefixes[st];

		size_t codeInputIndex = 0;

		const size_t opcodeSize = cmdCode->OpcodeSize;
		for (size_t st = 0; st < opcodeSize; st++)
			cmd->Code.Code[codeOutputIndex++] = cmdCode->Code[codeInputIndex++];

		size_t  lastOpcodeByte = codeOutputIndex - 1;
		uint8_t w = 0;

		assert(cmdCode->OpcodeLastByteType == AsmByteType::Code ||
			   cmdCode->OpcodeLastByteType == AsmByteType::CodeW);

		if (cmdCode->ModeRmSize > 0)
		{
			assert(cmdCode->ModeRmSize == 1);

			uint8_t modeRmByte = AsmGetModeRmByte(cmd, cmdCode, cmdCode->Code[codeInputIndex++], &w);

			cmd->Code.Code[codeOutputIndex++] = modeRmByte;
		}

		if (cmdCode->OpcodeLastByteType == AsmByteType::CodeW)
		{
			// CodeW байт:
			//  7 6 5 4 3 2 1 0
			// [-----code---- w ]
			// По умолчанию w == 0.
			// Если w == 1, то устанавливаем 0-ой бит.
			cmd->Code.Code[lastOpcodeByte] += w;
		}

		assert(cmdCode->SibSize == 0);

		if (cmdCode->ImmSize > 0)
		{
			assert(cmdCode->ImmSize == 1 || cmdCode->ImmSize == 2 || cmdCode->ImmSize == 4);

			if (cmd->Arg1.Type == AsmArgType::Label)
			{
				uint32_t address = comp->AsmRep.CurCmdOffset + codeOutputIndex + cmdCode->ImmSize;
				memcpy(cmd->Code.Code + codeOutputIndex, &address, cmdCode->ImmSize);

				LabelTableAddInsert(&comp->AsmRep.LabelTable, cmd->Arg1.Label, (int*)(cmd->Code.Code + codeOutputIndex));
			}
			else if (cmd->Arg1.Type == AsmArgType::Imm || cmd->Arg1.Type == AsmArgType::Mem)
			{
				memcpy(cmd->Code.Code + codeOutputIndex, &cmd->Arg1.Imm, cmdCode->ImmSize);
			}
			else // if (cmd->Arg2.Type == AsmArgType::Imm)
			{
				assert(cmd->Arg2.Type == AsmArgType::Imm || cmd->Arg2.Type == AsmArgType::Mem);
				memcpy(cmd->Code.Code + codeOutputIndex, &cmd->Arg2.Imm, cmdCode->ImmSize);
			}

			codeOutputIndex += cmdCode->ImmSize;
		}

		cmd->Code.CodeSize = codeOutputIndex;

		comp->AsmRep.CurCmdOffset += cmd->Code.CodeSize;
	}

	comp->AsmRep.JitBufferSize = comp->AsmRep.CurCmdOffset;

	return ProgramStatus::Ok;
}

/**
 * @brief  Ищет запись для кодировки команды с аргументами в массиве кодировок AsmCmdCodes.
 * 
 * Записи с одинаковым типом команды в массиве кодировок лежат последовательно.
 * Сначала находится начало записей с данным типом команд. 
 * Затем находится команда с данными аргументами.
 * 
 * @param  type Искомый тип команды.
 * 
 * @return Индекс записи кодировки данной команды. Если запись не была найдена, то AsmCmdCodesNull.
*/
static size_t AsmFindCmdCode(const AsmCmd* const cmd)
{
	assert(cmd);

	size_t cmdRecordsStart = AsmCmdCodesNull;

	for (size_t st = 0; st < AsmCmdCodesCount; st++)
	{
		// Возвращаем указатель на первый 
		if (AsmCmdCodes[st].Type == cmd->Type)
		{
			cmdRecordsStart = st;
			break;
		}
	}

	if (cmdRecordsStart == AsmCmdCodesNull)
		return AsmCmdCodesNull;

	for (size_t st = cmdRecordsStart; st < AsmCmdCodesCount; st++)
	{
		if (AsmCmdCodes[st].Type != cmd->Type)
			return AsmCmdCodesNull;

		if (AsmCmdCodes[st].Arg1Type == cmd->Arg1.Type &&
			AsmCmdCodes[st].Arg2Type == cmd->Arg2.Type)
			return st;
	}

	return AsmCmdCodesNull;
}

/**
 * @brief  Получить кодировку регистра. В Opcode последний байт типа CodeW.
 * 
 * @param  reg   Тип кодируемого регистра.
 * @param  out_w Выходной бит w. Показывает, занимают данные 1 байт или являются полноразмерными (2 или 4 байта).
 * 
 * @return Кодировка регистра (3 младших бита).
*/
static uint8_t AsmGetRegEnc32W(const AsmRegType reg, uint8_t* const out_w)
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
			assert(!"Not implemented");
			return 0;
	}
}

/**
 * @brief  Получить кодировку регистра. В Opcode только байты типа Code.
 * 
 * @param reg Тип кодируемого регистра.
 * 
 * @return Кодировка регистра (3 младших бита).
*/
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

/**
 * @brief  Закодировать байт ModR/M.
 * 
 * @param cmd       Кодируемая команда в ассемблере.
 * @param code      Правила перевода команды в машинный код.
 * @param modRmByte Установленное стандартное значение в правилах перевода.
 * @param out_w     Выходной бит w. Показывает, занимают данные 1 байт или являются полноразмерными (2 или 4 байта).
 * 
 * @return Закодированный байт ModR/M.
*/
static uint8_t AsmGetModeRmByte(const AsmCmd* const cmd, const AsmCmdCode* const code, 
								  const uint8_t modRmByte, uint8_t* const out_w)
{
	assert(cmd);
	assert(code);
	assert(out_w);

	AsmModRmByte modeRmByte = {.Value = modRmByte};

	switch (code->ModeRmType)
	{
		case AsmByteType::CodeReg:
		{
			uint8_t    reg     = 0;
			AsmRegType regType = AsmRegType::null;

			if (cmd->Arg1.Type == AsmArgType::Reg)
				regType = cmd->Arg1.Reg;
			else
			{
				assert(cmd->Arg2.Type == AsmArgType::Reg);
				regType = cmd->Arg2.Reg;
			}
			
			if (code->OpcodeLastByteType == AsmByteType::Code)
				reg = AsmGetRegEnc32(cmd->Arg1.Reg);
			else
			{
				assert(code->OpcodeLastByteType == AsmByteType::CodeW);
				reg = AsmGetRegEnc32W(cmd->Arg1.Reg, out_w);
			}
			
			modeRmByte.CodeReg.reg = reg;
			break;
		}
		case AsmByteType::CodeRegReg:
		{
			assert(cmd->Arg1.Type == AsmArgType::Reg);
			assert(cmd->Arg2.Type == AsmArgType::Reg);

			uint8_t reg1 = 0, reg2 = 0;
			if (code->OpcodeLastByteType == AsmByteType::Code)
			{
				reg1 = AsmGetRegEnc32(cmd->Arg1.Reg);
				reg2 = AsmGetRegEnc32(cmd->Arg2.Reg);
			}
			else // if (cmdCode->OpcodeLastByteType == AsmByteType::CodeW)
			{
				uint8_t w2 = 0;
				reg1 = AsmGetRegEnc32W(cmd->Arg1.Reg, &w2);
				reg2 = AsmGetRegEnc32W(cmd->Arg2.Reg, out_w);
				assert(w2 == *out_w);
			}

			modeRmByte.CodeRegReg.reg1 = reg1;
			modeRmByte.CodeRegReg.reg2 = reg2;
			break;
		}
		case AsmByteType::ModeCodeRm:
		{
			assert(cmd->Arg1.Type == AsmArgType::Mem);
			assert(cmd->Arg2.Type == AsmArgType::Null);
			modeRmByte.Value = AsmEncodeModeCodeRm(&cmd->Arg1, modeRmByte.Value);
			break;
		}
		case AsmByteType::ModeRegRm:
		{
			assert(cmd->Arg1.Type == AsmArgType::Mem);
			assert(cmd->Arg2.Type == AsmArgType::Reg);
			modeRmByte.Value = AsmEncodeModeRegRm(&cmd->Arg1, &cmd->Arg2);
			break;
		}
		default:
		{
			assert(!"Not implemented");
			break;
		}
	}

	return modeRmByte.Value;
}

static uint8_t AsmEncodeModeCodeRm(const AsmArg* const mem, const uint8_t code)
{
	assert(mem);
	assert(mem->Type == AsmArgType::Mem);

	AsmModRmByte byte = {.Value = code};

	byte.ModeRegRm.mode = 0b10;
	if (mem->Reg != AsmRegType::null)
		byte.ModeRegRm.rm   = AsmGetRegEnc32(mem->Reg);

	return byte.Value;
}

static uint8_t AsmEncodeModeRegRm(const AsmArg* const mem, const AsmArg* const reg)
{
	assert(mem);
	assert(mem->Type == AsmArgType::Mem);
	assert(reg);
	assert(reg->Type == AsmArgType::Reg);

	AsmModRmByte byte = {};

	if (reg->Reg != AsmRegType::null)
		byte.ModeRegRm.reg = AsmGetRegEnc32(reg->Reg);

	byte.ModeRegRm.mode = 0b10;
	if (mem->Reg != AsmRegType::null)
		byte.ModeRegRm.rm   = AsmGetRegEnc32(mem->Reg);

	return byte.Value;
}

/**
 * @brief  Проверяет правильность записи ассемблерной команды.
 * 
 * @param cmd Ассемблерная команда.
 * 
 * @return ProgramStatus::Fault в случае ошибки, иначе ProgramStatus::Ok.
*/
static ProgramStatus AsmCheckCmdCorrect(const AsmCmd* const cmd)
{
	// Процессор intel не поддерживает команды:
	// pop  [esp + disp]
	// push [esp + disp]
	// fld  [esp + disp]
	if ((cmd->Type == AsmCmdType::pop || 
		 cmd->Type == AsmCmdType::push ||
		 cmd->Type == AsmCmdType::fld) &&
		(cmd->Arg1.Type == AsmArgType::Reg ||
		 cmd->Arg1.Type == AsmArgType::Mem) &&
		 cmd->Arg1.Reg == AsmRegType::esp)
	{
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///