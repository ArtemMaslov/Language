///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
//
// Поддержка компиляции ключевых слов.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 21:34 08.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>

#include "../../Modules/Logs/Logs.h"

#include "x86Compiler.h"

#include "DSL.h"
#include "compile_funct.h"
#include "compile_expr.h"
#include "compile_keywords.h"
#include "compile_aux.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus CompileReturn(x86Compiler* const comp, const ReturnNode* const returnNode)
{
	assert(returnNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	if (returnNode->Value)
	{
		status = CompileExpression(comp, returnNode->Value);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

		POP(REG1(eax));
	}

	MOV(REG1(esp), REG2(ebp));
	POP(REG1(ebp));
	RET();

	return ProgramStatus::Ok;
}

ProgramStatus CompileWhile(x86Compiler* const comp, const WhileNode* const whileNode)
{
	assert(comp);
	assert(whileNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const size_t labelIndex = comp->LabelIndex++;

	char* labelCond = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);
	char* labelEnd  = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);

	snprintf(labelCond, LabelBufferSize, "l_while_cond_%zd", labelIndex);
	snprintf(labelEnd,  LabelBufferSize, "l_while_end_%zd",  labelIndex);

	LABEL(labelCond);

	status = CompileExpression(comp, whileNode->Condition);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	POP(REG1(eax));
	CMP(IMM1(1), REG2(eax));
	JNE(labelEnd);

	status = CompileInstrBlock(comp, &whileNode->Body);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	JMP(labelCond);
	LABEL(labelEnd);

	return status;
}

ProgramStatus CompileIf(x86Compiler* const comp, const IfNode* const ifNode)
{
	assert(comp);
	assert(ifNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const size_t labelIndex = comp->LabelIndex++;

	char* labelFalse = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);
	char* labelEnd   = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);

	snprintf(labelFalse, LabelBufferSize, "l_if_false_%zd", labelIndex);
	snprintf(labelEnd,   LabelBufferSize, "l_if_end_%zd",   labelIndex);
			
	status = CompileExpression(comp, ifNode->Condition);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	POP(REG1(eax));
	CMP(IMM1(1), REG2(eax));
	JNE(labelFalse);

	status = CompileInstrBlock(comp, &ifNode->TrueBlock);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	JMP(labelEnd);
	LABEL(labelFalse);

	status = CompileInstrBlock(comp, &ifNode->FalseBlock);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	LABEL(labelEnd);

	return status;
}

char OutputFormatStr[] = "%f\n";
char InputFormatStr[]  = "%f";

ProgramStatus CompileOutput(x86Compiler* const comp, const OutputNode* const outputNode)
{
	assert(comp);
	assert(outputNode);

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	ProgramStatus status = CompileExpression(comp, outputNode->Output);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	MOV(REG1(eax), REG2(esp));
	FLD(MEM1(eax, 0));
	SUB(REG1(esp), IMM2(4));
	FSTPD(MEM1(eax, -4));

	PUSH(IMM1((int)(OutputFormatStr)));

	CALL("printf");
	ADD(REG1(esp), IMM2((int)(3 * 4)));

	return status;
}

ProgramStatus CompileInput(x86Compiler* const comp, const InputNode* const inputNode)
{
	assert(comp);
	assert(inputNode);

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	int memOffset = 0; 
    ProgramStatus status = IdentifierGetMemoryOffset(comp, inputNode->Input->NameId, &memOffset);
    if (status != ProgramStatus::Ok)
    {
        TRACE_ERROR();
        return status;
    }

	MOV(REG1(eax), REG2(ebp));
	ADD(REG1(eax), IMM2(memOffset));
	PUSH(REG1(eax));
	PUSH(IMM1((int)InputFormatStr));
	CALL("scanf");
	ADD(REG1(esp), IMM2(2 * 4));

	return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///