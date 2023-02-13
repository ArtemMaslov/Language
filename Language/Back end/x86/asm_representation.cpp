///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль ассемблерного представления для архитектуры Intel x86 (32-разряда).
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 19:54 13.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../../Modules/TargetOS.h"
#include "../../Modules/Logs/Logs.h"

#if defined(WINDOWS)
#include <Windows.h>
#include <memoryapi.h>
#elif defined(LINUX)
#include <sys/mman.h>
#endif

#include "x86Compiler.h"

#include "DSL.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define ASM_CMD(cmd, name, ...) name,

/// Массив символьных представлений команд.
static const char* const AsmCommandNames[] =
{
#include "asm_cmd_names.inc"
};
#undef ASM_CMD

#define ASM_REG(reg) #reg,

/// Массив символьных представлений регистров.
static const char* const AsmRegNames[] =
{
#include "asm_regs.inc"
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

/// Массив кодировок команд.
/// Содержит информацию о том, как перевести команду в машинный код.
static const AsmCmdCode AsmCmdCodes[] = 
{
	{
		.Type = AsmCmdType::null
	},
#include "asm_cmd_codes.inc"
};
#undef ASM_CMD

/// Количество записей в массиве кодировок команд.
static const size_t AsmCmdCodesCount = sizeof(AsmCmdCodes) / sizeof(AsmCmdCode);
/// Индекс нулевой записи. В массиве кодировок первая запись является пустой.
static const size_t AsmCmdCodesNull = 0;

/// Количество значащих цифр в адресе команды.
static const int    AsmListingAddressSize = 7;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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
static size_t AsmFindCmdCode(const AsmCmd* const cmd);

/**
 * @brief  Получить кодировку регистра. В Opcode последний байт типа CodeW.
 * 
 * @param  reg   Тип кодируемого регистра.
 * @param  out_w Выходной бит w. Показывает, занимают данные 1 байт или являются полноразмерными (2 или 4 байта).
 * 
 * @return Кодировка регистра (3 младших бита).
*/
static uint8_t AsmGetRegEnc32W(const AsmRegType reg, uint8_t* const out_w);

/**
 * @brief  Получить кодировку регистра. В Opcode только байты типа Code.
 * 
 * @param reg Тип кодируемого регистра.
 * 
 * @return Кодировка регистра (3 младших бита).
*/
static uint8_t AsmGetRegEnc32(const AsmRegType reg);

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
								   const uint8_t modRmByte, uint8_t* const out_w);

/**
 * @brief  Проверяет правильность записи ассемблерной команды.
 * 
 * @param cmd Ассемблерная команда.
 * 
 * @return ProgramStatus::Fault в случае ошибки, иначе ProgramStatus::Ok.
*/
static ProgramStatus AsmCheckCmdCorrect(const AsmCmd* const cmd);

/**
 * @brief  Закодировать поле ModeRegRM.
 * 
 * @param mem Аргумент, содержащий адрес памяти.
 * @param reg Аргумент, содержащий регистр.
 * 
 * @return Закодированный байт.
 */
static uint8_t AsmEncodeModeRegRm(const AsmArg* const mem, const AsmArg* const reg);

/**
 * @brief  Закодировать поле ModeCodeRM.
 * 
 * @param mem  Аргумент, содержащий адрес памяти.
 * @param code Значение поля определяется спецификацией команд процессора.
 * 
 * @return Закодированный байт.
 */
static uint8_t AsmEncodeModeCodeRm(const AsmArg* const mem, const uint8_t code);

/**
 * @brief  Записать в файл листинга ассемблерного представления аргумент команды.
 * 
 * @param arg        Указатель на аргумент команды.
 * @param buffer     Указатель на выходной буфер.
 * @param BufferSize Размер выходного буфера форматирования.
 */
static void AsmWriteListingArg(const AsmArg* const arg, char* const buffer, const size_t BufferSize);

/**
 * @brief  Преобразовать число в строку (в двоичном формате) и записать результат в буфер. 
 * 
 * @param num    Преобразуемое число.
 * @param buffer Выходной буфер.
 */
static void ToBin(uint8_t num, char** const buffer);

/**
 * @brief  Преобразовать число в строку (в шестнадцатеричном формате) и записать результат в буфер. 
 * 
 * @param num    Преобразуемое число.
 * @param buffer Выходной буфер.
 */
static void ToHex(uint8_t num, char** const buffer);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AsmRepConstructor(AsmRepresentation* const asmRep)
{
	assert(asmRep);

	ProgramStatus status = ProgramStatus::Ok;

	asmRep->CurCmdOffset = 0;

	status = ExtArrayConstructor(&asmRep->Commands, sizeof(AsmCmd), AsmRepCommandsDefaultCount);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	status = ExtHeapConstructor(&asmRep->Labels, AsmLabelsDefaultCount);
	if (status != ProgramStatus::Ok)
	{
		ExtArrayDestructor(&asmRep->Commands);
		TRACE_ERROR();
		return status;
	}

	status = LabelTableConstructor(&asmRep->LabelTable);
	if (status != ProgramStatus::Ok)
	{
		ExtArrayDestructor(&asmRep->Commands);
		ExtHeapDestructor(&asmRep->Labels);
		TRACE_ERROR();
		return status;
	}

	return ProgramStatus::Ok;
}

void AsmRepDestructor(AsmRepresentation* const asmRep)
{
	assert(asmRep);

	ExtArrayDestructor(&asmRep->Commands);
	ExtHeapDestructor(&asmRep->Labels);
	LabelTableDestructor(&asmRep->LabelTable);

	if (asmRep->JitBuffer)
	{
#if defined(WINDOWS)
		VirtualFree(asmRep->JitBuffer, 0, MEM_RELEASE);
#elif defined(LINUX)
		if (munmap(asmRep->JitBuffer, asmRep->JitBufferSize) == -1)
		{
			LOG_ERR("[Linux] Ошибка munmap() при освобождении памяти.");
		}
#endif
	}
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AsmTranslateCmdsToByteCode(AsmRepresentation* const asmRep)
{
	assert(asmRep);

	ProgramStatus status = ProgramStatus::Ok;

	AsmLabelTable* const labelTable = &asmRep->LabelTable;
	const ExtArray* const commands = &asmRep->Commands;
	const size_t cmdCount = asmRep->Commands.Size;

	for (size_t cmdIndex = 0; cmdIndex < cmdCount; cmdIndex++)
	{
		AsmCmd* cmd = (AsmCmd*)ExtArrayGetElemAt(commands, cmdIndex);

		if (cmd->Type == AsmCmdType::label)
		{
			status = LabelTableAddLabel(labelTable, cmd->Arg1.Label, asmRep->CurCmdOffset);
			if (status != ProgramStatus::Ok)
			{
				TRACE_ERROR();
				return status;
			}
			continue;
		}

		size_t cmdCodeIndex = AsmFindCmdCode(cmd);
		if (cmdCodeIndex == AsmCmdCodesNull)
		{
			TRACE_ERROR();
			return ProgramStatus::Fault;
		}
		
		status = AsmCheckCmdCorrect(cmd);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

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
				uint32_t address = asmRep->CurCmdOffset + codeOutputIndex + cmdCode->ImmSize;
				memcpy(cmd->Code.Code + codeOutputIndex, &address, cmdCode->ImmSize);

				status = LabelTableAddInsert(labelTable, cmd->Arg1.Label, (int*)(cmd->Code.Code + codeOutputIndex));
				if (status != ProgramStatus::Ok)
				{
					TRACE_ERROR();
					return status;
				}
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

		asmRep->CurCmdOffset += cmd->Code.CodeSize;
	}

	asmRep->JitBufferSize = asmRep->CurCmdOffset;

	return status;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AsmCreateCommand(AsmRepresentation* const asmRep, const AsmCmdType type, const AsmArg* const arg1, const AsmArg* const arg2)
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
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}
	
	return status;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AsmWriteListingFile(const AsmRepresentation* const asmRep, const char* const fileName, const AsmListingType type)
{
	assert(asmRep);
	assert(fileName);

	ProgramStatus status = ProgramStatus::Ok;

	FILE* const file = fopen(fileName, "w");
	if (!file)
	{
		LOG_F_ERR("Ошибка открытия файла \"%s\"", fileName);
		return ProgramStatus::Fault;
	}

	const ExtArray* const commands = &asmRep->Commands;
	const size_t commandsCount = commands->Size;

	size_t cmdAddr = 0;

	for (size_t st = 0; st < commandsCount; st++)
	{
		AsmCmd* const asmCmd = (AsmCmd* const)ExtArrayGetElemAt(commands, st);

		if (asmCmd->Type == AsmCmdType::label)
		{
			fprintf(file, "%s:\n", asmCmd->Arg1.Label);
		}
		else
		{
			const size_t bufHexSize    = 30;
			char  bufHex[bufHexSize]   = "";
			char* bufHexPtr            = bufHex;

			const size_t bufCodeSize   = 256;
			char  bufCode[bufCodeSize] = "";
			char* bufCodePtr           = bufCode;

			const size_t bufCmdSize    = 20;
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
				const char* const cmdName = AsmGetCommandName(asmCmd->Type);

				if (asmCmd->Arg1.Type != AsmArgType::Null && asmCmd->Arg2.Type != AsmArgType::Null)
				{
					const size_t argBufferSize = 20;
					char  arg1[argBufferSize]  = "";
					char  arg2[argBufferSize]  = "";

					AsmWriteListingArg(&asmCmd->Arg1, arg1, argBufferSize);
					AsmWriteListingArg(&asmCmd->Arg2, arg2, argBufferSize);

					if (snprintf(bufCmd, bufCmdSize, "%s %s, %s", cmdName, arg1, arg2) < 0)
					{
						LOG_F_ERR("Ошибка форматирования строки snprintf."
							"bufCmd = \"%s\", cmdName = \"%s\", arg1 = \"%s\", arg2 = \"%s\".", 
							bufCmd, cmdName, arg1, arg2);
					}
				}
				else if (asmCmd->Arg1.Type != AsmArgType::Null)
				{
					const size_t argBufferSize = 20;
					char  arg[argBufferSize]  = "";

					AsmWriteListingArg(&asmCmd->Arg1, arg, argBufferSize);

					if (snprintf(bufCmd, bufCmdSize, "%s %s", cmdName, arg) < 0)
					{
						LOG_F_ERR("Ошибка форматирования строки snprintf."
							"bufCmd = \"%s\", cmdName = \"%s\", arg = \"%s\".", 
							bufCmd, cmdName, arg);
					}
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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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
		byte.ModeRegRm.rm = AsmGetRegEnc32(mem->Reg);

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
		byte.ModeRegRm.rm = AsmGetRegEnc32(mem->Reg);

	return byte.Value;
}

static ProgramStatus AsmCheckCmdCorrect(const AsmCmd* const cmd)
{
	assert(cmd);

	// Архитектура x86 не поддерживает команды:
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
		LOG_F_ERR("Использование не существующей команды. Type = \"%d\", argType = \"%d\", argReg = \"%d\".", 
			(int)cmd->Type, (int)cmd->Arg1.Type, (int)cmd->Arg1.Reg);
		return ProgramStatus::Fault;
	}

	return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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

static void AsmWriteListingArg(const AsmArg* const arg, char* const buffer, const size_t BufferSize)
{
	assert(arg);
	assert(buffer);

	const int imm        = arg->Imm;
	const AsmRegType reg = arg->Reg;

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