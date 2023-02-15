///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
//
// Поддержка компиляции функций.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 21:35 08.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <string.h>

#include "../../Modules/Logs/Logs.h"

#include "x86Compiler.h"

#include "intrinsic_funct.h"
#include "DSL.h"
#include "compile_funct.h"
#include "compile_expr.h"
#include "compile_keywords.h"
#include "compile_aux.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus CompileFunctDef(x86Compiler* const comp, const FunctDefNode* const functNode)
{
	assert(comp);
	assert(functNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	comp->CurrentFunction = functNode;

	if (IdentifierTableConstructor(&comp->CurrentVariablesScope) != IdentifierError::NoErrors)
	{
		TRACE_ERROR();
		return ProgramStatus::Fault;
	}

	const Identifier* const id = IdentifierGetById(comp->Ast->Identifiers, functNode->NameId);
	if (!id)
	{
		TRACE_ERROR();
		IdentifierTableDestructor(&comp->CurrentVariablesScope);
		return ProgramStatus::Fault;
	}

	const char* const functNodeName = id->Name;

	LABEL(functNodeName);
	PUSH(REG1(ebp));
	MOV(REG1(ebp), REG2(esp));
	// Потом вместо 0 подставиться размер локальных переменных.
	SUB(REG1(esp), IMM2(0));

	AsmCmd* lastCmd  = (AsmCmd*)ExtArrayGetElemAt(&asmRep->Commands, asmRep->Commands.Size - 1);
	int*    varCount = &lastCmd->Arg2.Imm;

	status = CompileInstrBlock(comp, &functNode->Body);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		IdentifierTableDestructor(&comp->CurrentVariablesScope);
		return status;
	}

	int stackDelta = (int)comp->CurrentVariablesScope.CompileIdentInited * 4;
	*varCount = stackDelta;

	MOV(REG1(esp), REG2(ebp));
	POP(REG1(ebp));
	RET();

	IdentifierTableDestructor(&comp->CurrentVariablesScope);

	return status;
}

ProgramStatus CompileFunctCall(x86Compiler* const comp, const FunctCallNode* const functCallNode)
{
	assert(comp);
	assert(functCallNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const ExtArray* const params = &functCallNode->Params;
	const size_t paramsCount = params->Size;

	for (int st = (int)(paramsCount - 1); st >= 0; st--)
	{
		FunctParamNode* param = (FunctParamNode*)ExtArrayGetElemAt(params, st);

		status = CompileExpression(comp, param->Value);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}
	}

	const Identifier* const id = IdentifierGetById(comp->Ast->Identifiers, functCallNode->NameId);
	if (!id)
	{
		TRACE_ERROR();
		return ProgramStatus::Fault;
	}
	const char* const functNodeName = id->Name;

	CALL(functNodeName);
	ADD(REG1(esp), IMM2((int)(paramsCount * 4)));

	return status;
}

ProgramStatus CompileInstrinsicFunction(x86Compiler* const comp, FunctCallNode* const functCallNode)
{
	assert(comp);
	assert(functCallNode);

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const Identifier* const id = IdentifierGetById(comp->Ast->Identifiers, functCallNode->NameId);
	if (!id)
	{
		return ProgramStatus::Fault;
	}
	const char* const functNodeName = id->Name;

	if (strncmp(functNodeName, IntrinSqrt.Name, IntrinSqrt.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 1);

		FunctParamNode* const arg = (FunctParamNode*)ExtArrayGetElemAt(&functCallNode->Params, 0);

		ProgramStatus status = CompileExpression(comp, arg->Value);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}
		
		CALL("sqrtf");
		MOV(REG1(eax), REG2(esp));
		FSTP(MEM1(eax, 0));
		POP(REG1(eax));

		return status;
	}
	else if (strncmp(functNodeName, IntrinInt.Name, IntrinInt.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 1);

		FunctParamNode* const value = (FunctParamNode*)ExtArrayGetElemAt(&functCallNode->Params, 0);

		ProgramStatus status = CompileExpression(comp, value->Value);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

		MOV(REG1(eax), REG2(esp));
		FLD(MEM1(eax, 0));
		FRND();
		FISTP(MEM1(eax, 0));

		return ProgramStatus::Ok;
	}
	else if (strncmp(functNodeName, IntrinLight.Name, IntrinLight.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 0);

		PUSH(IMM1('*'));
		CALL("putchar");
		ADD(REG1(esp), IMM2(4));

		return ProgramStatus::Ok;
	}
	else if (strncmp(functNodeName, IntrinDark.Name, IntrinDark.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 0);

		PUSH(IMM1(' '));
		CALL("putchar");
		ADD(REG1(esp), IMM2(4));

		return ProgramStatus::Ok;
	}
	else
	{
		return CompileFunctCall(comp, functCallNode);
	}
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///