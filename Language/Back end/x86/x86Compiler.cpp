///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
// 
// Основные функции, вызываемые из внешних модулей.
//
// Версия: 1.0.1.0
// Дата последнего изменения: 14:54 13.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../Modules/TargetOS.h"

#if defined(WINDOWS)
#include <Windows.h>
#include <memoryapi.h>
#elif defined(LINUX)
#include <sys/mman.h>
#endif

#include <math.h>

#include "../../Modules/Logs/Logs.h"
#include "../../Modules/ErrorsHandling.h"

#include "x86Compiler.h"
#include "../../Front end/Lexer/Identifier.h"

#include "asm_representation.h"

#include "DSL.h"
#include "compile_aux.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus x86CompilerConstructor(x86Compiler* const comp, AST* const ast)
{
	assert(comp);
	assert(ast);

	comp->Ast = ast;

	return AsmRepConstructor(&comp->AsmRep);
}

void x86CompilerDestructor(x86Compiler* const comp)
{
	assert(comp);

	AsmRepDestructor(&comp->AsmRep);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus x86CompileToAsm(x86Compiler* const comp)
{
	assert(comp);

	AsmRepresentation* const asmRep = &comp->AsmRep;

	AsmArg arg1 = {};
	AsmArg arg2 = {};

	CALL("main");
	//MOV(REG1(eax), IMM2(0));
	RET();

	ProgramStatus status = CompileConstrNodes(comp);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	return status;
}

ProgramStatus x86TranslateToByteCode(x86Compiler* const comp)
{
	assert(comp);

	return AsmTranslateCmdsToByteCode(&comp->AsmRep);
}

ProgramStatus x86CompileJitBuffer(x86Compiler* const comp)
{
	assert(comp);

	const AsmRepresentation* const asmRep = &comp->AsmRep;
	ProgramStatus status = ProgramStatus::Ok;

#if defined(WINDOWS)
	uint8_t* jitBuffer = (uint8_t*)VirtualAlloc(nullptr, asmRep->JitBufferSize, MEM_COMMIT | MEM_RESERVE, 
												PAGE_READWRITE);
	
	if (!jitBuffer)
	{
		LOG_F_ERR("[Windows] Ошибка выделения памяти VirtualAlloc(). Запрошено %zd байт.", asmRep->JitBufferSize);
		return ProgramStatus::Fault;
	}

#elif defined(LINUX)
	uint8_t* jitBuffer = (uint8_t*)mmap(NULL, asmRep->JitBufferSize, PROT_READ | PROT_WRITE,
										MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (jitBuffer == MAP_FAILED)
	{
		LOG_F_ERR("[Linux] Ошибка выделения памяти mmap(). Запрошено %zd байт.", asmRep->JitBufferSize);
		return ProgramStatus::Fault;
	}
#endif

	status = LabelTableAddLabel(&comp->AsmRep.LabelTable, "printf",  (size_t)printf   - (size_t)jitBuffer);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	status = LabelTableAddLabel(&comp->AsmRep.LabelTable, "scanf",   (size_t)scanf    - (size_t)jitBuffer);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	status = LabelTableAddLabel(&comp->AsmRep.LabelTable, "sqrtf",   (size_t)sqrtf    - (size_t)jitBuffer);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	status = LabelTableAddLabel(&comp->AsmRep.LabelTable, "putchar", (size_t)putchar  - (size_t)jitBuffer);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	LabelTableWrite(&comp->AsmRep.LabelTable);

	const ExtArray* const cmds = &asmRep->Commands;
	const size_t cmdCount = asmRep->Commands.Size;

	uint8_t* copyBuf = jitBuffer;

	for (size_t st = 0; st < cmdCount; st++)
	{
		AsmCmd* cmd = (AsmCmd*)ExtArrayGetElemAt(cmds, st);

		if (cmd == nullptr)
		{
			TRACE_ERROR();
			return ProgramStatus::Fault;
		}

		memcpy(copyBuf, cmd->Code.Code, cmd->Code.CodeSize);

		copyBuf += cmd->Code.CodeSize;
	}

	comp->AsmRep.JitBuffer = jitBuffer;

	return ProgramStatus::Ok;
}

ProgramStatus x86DumpJIT(x86Compiler* const comp, const char* const fileName)
{
	assert(comp);

	FILE* file = fopen(fileName, "wb");

	if (!file)
	{
		LOG_F_ERR("Ошибка открытия файла \"%s\"", fileName);
		return ProgramStatus::Fault;
	}

	fwrite(comp->AsmRep.JitBuffer, sizeof(uint8_t), comp->AsmRep.JitBufferSize, file);

	fclose(file);

	return ProgramStatus::Ok;
}

ProgramStatus x86WriteListingFile(const x86Compiler* const comp, const char* const fileName, const AsmListingType type)
{
	return AsmWriteListingFile(&comp->AsmRep, fileName, type);
}

typedef void (*JitFunctPtr)(void);

ProgramStatus x86RunJIT(x86Compiler* const comp)
{
	assert(comp);
	assert(comp->AsmRep.JitBuffer);

#if defined(WINDOWS)
	DWORD oldProtect;
	if (!VirtualProtect(comp->AsmRep.JitBuffer, comp->AsmRep.JitBufferSize, PAGE_EXECUTE_READ, &oldProtect))
	{
		LOG_ERR("[Windows] Не удалось сбросить флаг защиты памяти от исполнения.");
		return ProgramStatus::Fault;
	}

#elif defined(LINUX)
	if (mprotect(comp->AsmRep.JitBuffer, comp->AsmRep.JitBufferSize, PROT_EXEC | PROT_READ) == -1)
	{
		LOG_ERR("[Linux] Не удалось сбросить флаг защиты памяти от исполнения.");
		return ProgramStatus::Fault;
	}

#endif

	JitFunctPtr functPtr = (JitFunctPtr)(comp->AsmRep.JitBuffer);

	functPtr();

 	return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///