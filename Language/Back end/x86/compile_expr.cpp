///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
//
// Поддержка компиляции выражений.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 21:35 08.02.2023
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

ProgramStatus CompileExpression(x86Compiler* const comp, const ExpressionNode* const exprNode)
{
	assert(comp);
	assert(exprNode);

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	ProgramStatus status = ProgramStatus::Ok;

	switch (exprNode->Type)
	{
		case AstNodeType::BinaryOperator:
			status = CompileBinaryOperator(comp, exprNode->Node.BinaryOperator);
			break;

		case AstNodeType::UnaryOperator:
			status = CompileUnaryOperator(comp, exprNode->Node.UnaryOperator);
			break;

		case AstNodeType::Number:
			status = CompileNumber(comp, exprNode->Node.Number);
			break;

		case AstNodeType::FunctCall:
			status = CompileInstrinsicFunction(comp, exprNode->Node.FunctCall);

			PUSH(REG1(eax));

			break;

		case AstNodeType::Variable:
			status = CompileVariable(comp, exprNode->Node.Variable);
			break;

		default:
			assert(!"Unknown");
			LOG_F_ERR("Не известный тип \"%d\" узла АСД.", (int)exprNode->Type);
			break;
	}

	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}
	return status;
}

ProgramStatus CompileUnaryOperator(x86Compiler* const comp, const UnaryOperatorNode* const unOperNode)
{
	assert(comp);
	assert(unOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	status = CompileExpression(comp, unOperNode->Operand);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	switch (unOperNode->Operator)
	{
		case OperatorType::Not:
			status = CompileNotOperator(comp, unOperNode);
			if (status != ProgramStatus::Ok)
			{
				TRACE_ERROR();
				return status;
			}
			return status;
			break;

		case OperatorType::Subtraction:
			// Для целых чисел.
			//POP(REG1(eax));
			//NEG(REG1(eax));
			//PUSH(REG1(eax));
			
			// Для float.
			MOV(REG1(eax), REG2(esp));
			FLD(MEM1(eax, 0));
			FCHS();
			FSTP(MEM1(eax, 0));
			break;

		case OperatorType::Addition:
			// Игнорируем унарный плюс.
			break;

		default:
			assert(!"Error");
			break;
	}

	return status;
}

ProgramStatus CompileBinaryOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	status = CompileExpression(comp, binOperNode->LeftOperand);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	status = CompileExpression(comp, binOperNode->RightOperand);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	switch (binOperNode->Operator)
	{
		case OperatorType::Addition:
			// Для целых чисел.
			// POP(REG1(ebx));
			// POP(REG1(eax));
			// 
			// ADD(REG1(eax), REG2(ebx));
			// PUSH(REG1(eax));

			// Для float.
			MOV(REG1(eax), REG2(esp));
			FLD(MEM1(eax, 4));
			FLD(MEM1(eax, 0));
			FADDP();
			FSTP(MEM1(eax, 4));
			ADD(REG1(esp), IMM2(4));
			break;

		case OperatorType::Subtraction:
			// Для целых чисел.
			// POP(REG1(ebx));
			// POP(REG1(eax));
			// 
			// SUB(REG1(eax), REG2(ebx));
			// PUSH(REG1(eax));
			
			// Для float.
			MOV(REG1(eax), REG2(esp));
			FLD(MEM1(eax, 4));
			FLD(MEM1(eax, 0));
			FSUBP();
			FSTP(MEM1(eax, 4));
			ADD(REG1(esp), IMM2(4));
			break;

		case OperatorType::Multiplication:
			// Для целых чисел.
			// POP(REG1(ebx));
			// POP(REG1(eax));
			// CDQ();
			// 
			// IMUL(REG1(ebx));
			// PUSH(REG1(eax));
			
			// Для float.
			MOV(REG1(eax), REG2(esp));
			FLD(MEM1(eax, 4));
			FLD(MEM1(eax, 0));
			FMULP();
			FSTP(MEM1(eax, 4));
			ADD(REG1(esp), IMM2(4));
			break;

		case OperatorType::Division:
			// Для целых чисел.
			// POP(REG1(ebx));
			// POP(REG1(eax));
			// CDQ();
			// 
			// IDIV(REG1(ebx));
			// PUSH(REG1(eax));
			
			// Для float.
			MOV(REG1(eax), REG2(esp));
			FLD(MEM1(eax, 4));
			FLD(MEM1(eax, 0));
			FDIVP();
			FSTP(MEM1(eax, 4));
			ADD(REG1(esp), IMM2(4));
			break;

		case OperatorType::Equal:
		case OperatorType::NotEqual:
		case OperatorType::Greater:
		case OperatorType::GreaterEqual:
		case OperatorType::Less:
		case OperatorType::LessEqual:
			status = CompileCompareOperator(comp, binOperNode);
			if (status != ProgramStatus::Ok)
			{
				TRACE_ERROR();
				return status;
			}
			return status;

		case OperatorType::And:
			status = CompileAndOperator(comp, binOperNode);
			if (status != ProgramStatus::Ok)
			{
				TRACE_ERROR();
				return status;
			}
			return status;
			break;

		case OperatorType::Or:
			status = CompileOrOperator(comp, binOperNode);
			if (status != ProgramStatus::Ok)
			{
				TRACE_ERROR();
				return status;
			}
			return status;
			break;

		default:
			assert(!"Unknown");
			break;
	}

	return status;
}

ProgramStatus CompileNotOperator(x86Compiler* const comp, const UnaryOperatorNode* const unOperNode)
{
	assert(comp);
	assert(unOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const size_t labelIndex = comp->LabelIndex++;

	char* labelFalse = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);
	char* labelEnd   = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);

	snprintf(labelFalse, LabelBufferSize, "l_not_false_%zd", labelIndex);
	snprintf(labelEnd,   LabelBufferSize, "l_not_end_%zd",   labelIndex);

	POP(REG1(eax));
	CMP(IMM1(0), REG2(eax));
	JNE(labelFalse);

	PUSH(IMM1(1));
	JMP(labelEnd);

	LABEL(labelFalse);
	PUSH(IMM1(0));

	LABEL(labelEnd);

	return status;
}

ProgramStatus CompileAndOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const size_t labelIndex = comp->LabelIndex++;

	char* labelFalse = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);
	char* labelEnd   = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);

	snprintf(labelFalse, LabelBufferSize, "l_and_false_%zd", labelIndex);
	snprintf(labelEnd,   LabelBufferSize, "l_and_end_%zd",   labelIndex);

	POP(REG1(eax));
	CMP(IMM1(1), REG2(eax));
	JNE(labelFalse);

	POP(REG1(eax));
	CMP(IMM1(1), REG2(eax));
	JNE(labelFalse);
					
	PUSH(IMM1(1));
	JMP(labelEnd);

	LABEL(labelFalse);
	PUSH(IMM1(0));
	LABEL(labelEnd);

	return status;
}

ProgramStatus CompileOrOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const size_t labelIndex = comp->LabelIndex++;

	char* labelTrue  = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);
	char* labelFalse = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);
	char* labelEnd   = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);

	snprintf(labelTrue,  LabelBufferSize, "l_or_true_%zd",  labelIndex);
	snprintf(labelFalse, LabelBufferSize, "l_or_false_%zd", labelIndex);
	snprintf(labelEnd,   LabelBufferSize, "l_or_end_%zd",   labelIndex);

	POP(REG1(eax));
	CMP(IMM1(1), REG2(eax));
	JE(labelTrue);

	POP(REG1(eax));
	CMP(IMM1(1), REG2(eax));
	JNE(labelFalse);

	LABEL(labelTrue);
	PUSH(IMM1(1));
	JMP(labelEnd);

	LABEL(labelFalse);
	PUSH(IMM1(0));
	LABEL(labelEnd);

	return status;
}

ProgramStatus CompileCompareOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	AsmCmdType type = AsmCmdType::null;
	const char*  labelName  = nullptr;
	const size_t labelIndex = comp->LabelIndex++;

	switch (binOperNode->Operator)
	{
		case OperatorType::Equal:
			type = AsmCmdType::jne;
			labelName = CompEqualLabelName;
			break;

		case OperatorType::NotEqual:
			type = AsmCmdType::je;
			labelName = CompNotEqualLabelName;
			break;

		case OperatorType::Greater:
			type = AsmCmdType::jbe;
			labelName = CompGreaterLabelName;
			break;

		case OperatorType::GreaterEqual:
			type = AsmCmdType::jb;
			labelName = CompGreaterEqualLabelName;
			break;

		case OperatorType::Less:
			type = AsmCmdType::jae;
			labelName = CompLessLabelName;
			break;

		case OperatorType::LessEqual:
			type = AsmCmdType::ja;
			labelName = CompLessEqualLabelName;
			break;

		default:
			assert(!"Error");
			break;
	}

	char* labelFalse = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);
	char* labelEnd   = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);

	snprintf(labelFalse, LabelBufferSize, "l_%s_false_%zd", labelName, labelIndex);
	snprintf(labelEnd,   LabelBufferSize, "l_%s_end_%zd",   labelName, labelIndex);

	// Для целых чисел.
	// POP(REG1(ebx));
	// POP(REG1(eax));
	// CMP(REG1(eax), REG2(ebx));
	MOV(REG1(eax), REG2(esp));
	FLD(MEM1(eax, 0));
	FLD(MEM1(eax, 4));
	ADD(REG1(esp), IMM2(8));
	FCOMPP();

	AsmCreateCommand(asmRep, type, LABEL_ARG(labelFalse));

	PUSH(IMM1(1));
	JMP(labelEnd);

	LABEL(labelFalse);
	PUSH(IMM1(0));

	LABEL(labelEnd);

	return status;
}

ProgramStatus CompileNumber(x86Compiler* const comp, const double number)
{
	assert(comp);

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};

	const float fnum = (float)number;
	PUSH(IMM1(*(int*)&fnum));

	return ProgramStatus::Ok;
}

ProgramStatus CompileVariable(x86Compiler* const comp, const VariableNode* const varNode)
{
	assert(comp);
	assert(varNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRepresentation* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	bool varInit = false;
	int offset = 0;

    status = IdentifierGetMemoryOffset(comp, varNode->NameId, &offset, &varInit);
    if (status != ProgramStatus::Ok)
    {
        TRACE_ERROR();
        return status;
    }

	if (varNode->InitValue)
	{
		status = CompileExpression(comp, varNode->InitValue);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

		POP(MEM1(ebp, offset));
	}
	else
	{
		PUSH(MEM1(ebp, offset));
	}

	return status;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///