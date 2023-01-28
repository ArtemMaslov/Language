#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Windows.h>
#include <memoryapi.h>

#include <math.h>

#include "x86Compiler.h"
#include "../../Front end/Lexer/_identifier.h"

#include "_compiler_config.h"
#include "_intrinsic_funct.h"
#include "_asm_representation.h"

#include "DSL.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define CHECK_STATUS \
	assert(status == ProgramStatus::Ok)
	//if (status != ProgramStatus::Ok) \
	//	return status;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static const size_t IdentifierNull  = (size_t)(-1);
static const size_t LabelBufferSize = 32;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static ProgramStatus CompileFunctDef(x86Compiler* const comp, const FunctDefNode* const functNode);

static ProgramStatus CompileFunctCall(x86Compiler* const comp, const FunctCallNode* const functCallNode);

static ProgramStatus CompileInstrinsicFunction(x86Compiler* const comp, FunctCallNode* const functCallNode);

static ProgramStatus CompileWhile(x86Compiler* const comp, const WhileNode* const whileNode);

static ProgramStatus CompileIf(x86Compiler* const comp, const IfNode* const ifNode);

static ProgramStatus CompileOutput(x86Compiler* const comp, const OutputNode* const outputNode);

static ProgramStatus CompileInput(x86Compiler* const comp, const InputNode* const inputNode);

static ProgramStatus CompileExpression(x86Compiler* const comp, const ExpressionNode* const exprNode);

static ProgramStatus CompileUnaryOperator(x86Compiler* const comp, const UnaryOperatorNode* const unOperNode);

static ProgramStatus CompileNotOperator(x86Compiler* const comp, const UnaryOperatorNode* const unOperNode);

static ProgramStatus CompileBinaryOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode);

static ProgramStatus CompileAndOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode);

static ProgramStatus CompileOrOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode);

static int IdentifierGetMemoryOffset(x86Compiler* const comp, const int varId, bool* const outVarInitialization = nullptr);

static ProgramStatus CompileReturn(x86Compiler* const comp, const ReturnNode* const returnNode);

static ProgramStatus CompileNumber(x86Compiler* const comp, const double number);

static ProgramStatus CompileVariable(x86Compiler* const comp, const VariableNode* const varNode);

static ProgramStatus CompileInstrBlock(x86Compiler* const comp, const ExtArray* const instrBlock);

static ProgramStatus CompileConstrNodes(x86Compiler* const comp);

static ProgramStatus CompileCompareOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus x86CompilerConstructor(x86Compiler* const comp, AST* const ast)
{
	assert(comp);
	assert(ast);

	comp->Ast = ast;

	ProgramStatus status = AsmRepConstructor(&comp->AsmRep);

	return status;
}

void x86CompilerDestructor(x86Compiler* const comp)
{
	assert(comp);

	AsmRepDestructor(&comp->AsmRep);
}

ProgramStatus x86CompileAsmRep(x86Compiler* const comp)
{
	assert(comp);

	AsmRep* const asmRep = &comp->AsmRep;

	AsmArg arg1 = {};
	AsmArg arg2 = {};

	CALL("main");
	//MOV(REG1(eax), IMM2(0));
	RET();

	ProgramStatus status = CompileConstrNodes(comp);
	CHECK_STATUS;

	return status;
}

ProgramStatus x86CompileJitBuffer(x86Compiler* const comp)
{
	assert(comp);

	const AsmRep* const asmRep = &comp->AsmRep;

	uint8_t* jitBuffer = (uint8_t*)VirtualAlloc(nullptr, asmRep->JitBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!jitBuffer)
	{
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	LabelTableAddLabel(&comp->AsmRep.LabelTable, "printf",  (size_t)printf   - (size_t)jitBuffer);
	LabelTableAddLabel(&comp->AsmRep.LabelTable, "scanf",   (size_t)scanf    - (size_t)jitBuffer);
	LabelTableAddLabel(&comp->AsmRep.LabelTable, "sqrtf",   (size_t)sqrtf    - (size_t)jitBuffer);
	LabelTableAddLabel(&comp->AsmRep.LabelTable, "putchar", (size_t)putchar  - (size_t)jitBuffer);

	ProgramStatus status = LabelTableWrite(&comp->AsmRep.LabelTable);
	CHECK_STATUS;

	const ExtArray* const cmds = &asmRep->Commands;
	const size_t cmdCount = asmRep->Commands.Size;

	uint8_t* copyBuf = jitBuffer;

	for (size_t st = 0; st < cmdCount; st++)
	{
		AsmCmd* cmd = (AsmCmd*)ExtArrayGetElemAt(cmds, st);

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
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	fwrite(comp->AsmRep.JitBuffer, sizeof(uint8_t), comp->AsmRep.JitBufferSize, file);

	fclose(file);

	return ProgramStatus::Ok;
}

typedef void (*JitFunctPtr)(void);

ProgramStatus x86RunJIT(x86Compiler* const comp)
{
	assert(comp);
	assert(comp->AsmRep.JitBuffer);

	DWORD oldProtect;
	if (!VirtualProtect(comp->AsmRep.JitBuffer, comp->AsmRep.JitBufferSize, PAGE_EXECUTE_READ, &oldProtect))
	{
		assert(!"error");
		return ProgramStatus::Fault;
	}

	JitFunctPtr functPtr = (JitFunctPtr)(comp->AsmRep.JitBuffer);

	functPtr();

 	return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static ProgramStatus CompileFunctDef(x86Compiler* const comp, const FunctDefNode* const functNode)
{
	assert(comp);
	assert(functNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	comp->CurrentFunction = functNode;

	status = IdentifierTableConstructor(&comp->CurrentVariablesScope);
	CHECK_STATUS;

	const Identifier* const id = IdentifierGetById(&comp->Ast->IdentififerTable, functNode->NameId);
	if (!id)
	{
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
	CHECK_STATUS;

	int stackDelta = (int)comp->CurrentVariablesScope.CompileIdentInited * 4;
	*varCount = stackDelta;

	MOV(REG1(esp), REG2(ebp));
	POP(REG1(ebp));
	RET();

	status = IdentifierTableDestructor(&comp->CurrentVariablesScope);
	CHECK_STATUS;

	return status;
}

static ProgramStatus CompileFunctCall(x86Compiler* const comp, const FunctCallNode* const functCallNode)
{
	assert(comp);
	assert(functCallNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const ExtArray* const params = &functCallNode->Params;
	const size_t paramsCount = params->Size;

	for (int st = (int)(paramsCount - 1); st >= 0; st--)
	{
		FunctParamNode* param = (FunctParamNode*)ExtArrayGetElemAt(params, st);

		status = CompileExpression(comp, param->Value);
		CHECK_STATUS;
	}

	const Identifier* const id = IdentifierGetById(&comp->Ast->IdentififerTable, functCallNode->NameId);
	if (!id)
	{
		return ProgramStatus::Fault;
	}
	const char* const functNodeName = id->Name;

	CALL(functNodeName);
	ADD(REG1(esp), IMM2((int)(paramsCount * 4)));

	return status;
}

static ProgramStatus CompileReturn(x86Compiler* const comp, const ReturnNode* const returnNode)
{
	assert(returnNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	if (returnNode->Value)
	{
		status = CompileExpression(comp, returnNode->Value);
		CHECK_STATUS;

		POP(REG1(eax));
	}

	MOV(REG1(esp), REG2(ebp));
	POP(REG1(ebp));
	RET();

	return ProgramStatus::Ok;
}

static ProgramStatus CompileWhile(x86Compiler* const comp, const WhileNode* const whileNode)
{
	assert(comp);
	assert(whileNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const size_t labelIndex = comp->LabelIndex++;

	char* labelCond = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);
	char* labelEnd  = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);

	snprintf(labelCond, LabelBufferSize, "l_while_cond_%zd", labelIndex);
	snprintf(labelEnd,  LabelBufferSize, "l_while_end_%zd",  labelIndex);

	LABEL(labelCond);

	status = CompileExpression(comp, whileNode->Condition);
	CHECK_STATUS;

	POP(REG1(eax));
	CMP(IMM1(1), REG2(eax));
	JNE(labelEnd);

	status = CompileInstrBlock(comp, &whileNode->Body);
	CHECK_STATUS;

	JMP(labelCond);
	LABEL(labelEnd);

	return status;
}

static ProgramStatus CompileIf(x86Compiler* const comp, const IfNode* const ifNode)
{
	assert(comp);
	assert(ifNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const size_t labelIndex = comp->LabelIndex++;

	char* labelFalse = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);
	char* labelEnd   = (char*)ExtHeapAllocElem(&asmRep->Labels, LabelBufferSize);

	snprintf(labelFalse, LabelBufferSize, "l_if_false_%zd", labelIndex);
	snprintf(labelEnd,   LabelBufferSize, "l_if_end_%zd",   labelIndex);
			
	status = CompileExpression(comp, ifNode->Condition);
	CHECK_STATUS;

	POP(REG1(eax));
	CMP(IMM1(1), REG2(eax));
	JNE(labelFalse);

	status = CompileInstrBlock(comp, &ifNode->TrueBlock);
	CHECK_STATUS;

	JMP(labelEnd);
	LABEL(labelFalse);

	status = CompileInstrBlock(comp, &ifNode->FalseBlock);
	CHECK_STATUS;

	LABEL(labelEnd);

	return status;
}

char OutputFormatStr[] = "%f\n";
char InputFormatStr[]  = "%f";

static ProgramStatus CompileOutput(x86Compiler* const comp, const OutputNode* const outputNode)
{
	assert(comp);
	assert(outputNode);

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	ProgramStatus status = CompileExpression(comp, outputNode->Output);
	CHECK_STATUS;

	MOV(REG1(eax), REG2(esp));
	FLD(MEM1(eax, 0));
	SUB(REG1(esp), IMM2(4));
	FSTPD(MEM1(eax, -4));

	PUSH(IMM1((int)(OutputFormatStr)));

	CALL("printf");
	ADD(REG1(esp), IMM2((int)(3 * 4)));

	return status;
}

static ProgramStatus CompileInput(x86Compiler* const comp, const InputNode* const inputNode)
{
	assert(comp);
	assert(inputNode);

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	size_t memOffset = IdentifierGetMemoryOffset(comp, inputNode->Input->NameId);

	if (memOffset == IdentifierNull)
	{
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	MOV(REG1(eax), REG2(ebp));
	ADD(REG1(eax), IMM2(memOffset));
	PUSH(REG1(eax));
	PUSH(IMM1((int)InputFormatStr));
	CALL("scanf");
	ADD(REG1(esp), IMM2(2 * 4));

	return ProgramStatus::Ok;
}

static ProgramStatus CompileExpression(x86Compiler* const comp, const ExpressionNode* const exprNode)
{
	assert(comp);
	assert(exprNode);

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	ProgramStatus status = ProgramStatus::Ok;

	switch (exprNode->Type)
	{
		case AstNodeTypes::BinaryOperator:
			status = CompileBinaryOperator(comp, exprNode->Node.BinaryOperator);
			break;

		case AstNodeTypes::UnaryOperator:
			status = CompileUnaryOperator(comp, exprNode->Node.UnaryOperator);
			break;

		case AstNodeTypes::Number:
			status = CompileNumber(comp, exprNode->Node.Number);
			break;

		case AstNodeTypes::FunctCall:
			status = CompileInstrinsicFunction(comp, exprNode->Node.FunctCall);

			PUSH(REG1(eax));

			break;

		case AstNodeTypes::Variable:
			status = CompileVariable(comp, exprNode->Node.Variable);
			break;

		default:
			assert(!"Unknown");
			break;
	}

	CHECK_STATUS;
	return status;
}

static ProgramStatus CompileUnaryOperator(x86Compiler* const comp, const UnaryOperatorNode* const unOperNode)
{
	assert(comp);
	assert(unOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	status = CompileExpression(comp, unOperNode->Operand);
	CHECK_STATUS;

	const size_t labelIndex = comp->LabelIndex;

	switch (unOperNode->Operator)
	{
		case OperatorType::Not:
			status = CompileNotOperator(comp, unOperNode);
			CHECK_STATUS;
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

static ProgramStatus CompileBinaryOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	status = CompileExpression(comp, binOperNode->LeftOperand);
	CHECK_STATUS;

	status = CompileExpression(comp, binOperNode->RightOperand);
	CHECK_STATUS;

	const size_t labelIndex = comp->LabelIndex;

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
			CHECK_STATUS;
			return status;

		case OperatorType::And:
			status = CompileAndOperator(comp, binOperNode);
			CHECK_STATUS;
			return status;
			break;

		case OperatorType::Or:
			status = CompileOrOperator(comp, binOperNode);
			CHECK_STATUS;
			return status;
			break;

		default:
			assert(!"Unknown");
			break;
	}

	return status;
}

static ProgramStatus CompileNotOperator(x86Compiler* const comp, const UnaryOperatorNode* const unOperNode)
{
	assert(comp);
	assert(unOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
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

static ProgramStatus CompileAndOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
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

static ProgramStatus CompileOrOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
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

static ProgramStatus CompileCompareOperator(x86Compiler* const comp, const BinaryOperatorNode* const binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	AsmCmdType type = AsmCmdType::null;
	const char*  jmpType    = nullptr;
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

static int IdentifierGetMemoryOffset(x86Compiler* const comp, const int varId, bool* const outVarInitialization)
{
	assert(comp);
	// assert(outVarInitialization);

	IdentifierTable* const table = &comp->CurrentVariablesScope;

	// Проверяем, является ли переменная параметром функции.
	const ExtArray* const params = &comp->CurrentFunction->Params;
	const size_t paramsCount = comp->CurrentFunction->Params.Size;

	for (size_t st = 0; st < paramsCount; st++)
	{
		FunctParamNode* param = (FunctParamNode*)ExtArrayGetElemAt(params, st);

		if (param->NameId == varId)
		{
			if (outVarInitialization)
				*outVarInitialization = false;

			return 4 * (2 + (int)st);
		}
	}

	// Проверяем является ли переменная локальной в функции, а не её параметром.
	const ExtArray* const identifiers = &table->Table;
	const size_t identifiersCount = table->Table.Size;

	for (size_t st = 0; st < identifiersCount; st++)
	{
		Identifier* ident = (Identifier*)ExtArrayGetElemAt(identifiers, st);

		if (ident->Id == varId)
		{
			if (outVarInitialization)
				*outVarInitialization = false;

			return ident->CompileMemoryOffset;
		}
	}

	// Переменная раньше не встречалась. Создаём новую запись и выделяем память.

	const int offset = -4 * (int)(++table->CompileIdentInited);

	Identifier newIdent          = {};
	newIdent.CompileMemoryOffset = offset;
	newIdent.Id                  = varId;

	ProgramStatus status = ExtArrayAddElem(&table->Table, &newIdent);
	CHECK_STATUS;

	if (outVarInitialization)
		*outVarInitialization = true;

	return offset;
}

static ProgramStatus CompileNumber(x86Compiler* const comp, const double number)
{
	assert(comp);

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};

	const float fnum = (float)number;
	PUSH(IMM1(*(int*)&fnum));

	return ProgramStatus::Ok;
}

static ProgramStatus CompileVariable(x86Compiler* const comp, const VariableNode* const varNode)
{
	assert(comp);
	assert(varNode);

	ProgramStatus status = ProgramStatus::Ok;

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	bool varInit = false;

	const int offset = IdentifierGetMemoryOffset(comp, varNode->NameId, &varInit);

	if (varNode->InitValue)
	{
		status = CompileExpression(comp, varNode->InitValue);
		CHECK_STATUS;

		POP(MEM1(ebp, offset));
	}
	else
	{
		PUSH(MEM1(ebp, offset));
	}

	return status;
}

static ProgramStatus CompileInstrBlock(x86Compiler* const comp, const ExtArray* const instrBlock)
{
	assert(comp);
	assert(instrBlock);

	ProgramStatus status = ProgramStatus::Ok;

	const size_t instrCount = instrBlock->Size;

	for (size_t st = 0; st < instrCount; st++)
	{
		InstructionNode* instr = (InstructionNode*)ExtArrayGetElemAt(instrBlock, st);

		switch (instr->Type)
		{
			case AstNodeTypes::Variable:
				status = CompileVariable(comp, instr->Node.Variable);
				break;

			case AstNodeTypes::FunctCall:
				status = CompileInstrinsicFunction(comp, instr->Node.FunctCall);
				break;

			case AstNodeTypes::If:
				status = CompileIf(comp, instr->Node.If);
				break;

			case AstNodeTypes::While:
				status = CompileWhile(comp, instr->Node.While);
				break;

			case AstNodeTypes::Input:
				status = CompileInput(comp, instr->Node.Input);
				break;

			case AstNodeTypes::Output:
				status = CompileOutput(comp, instr->Node.Output);
				break;

			case AstNodeTypes::Return:
				status = CompileReturn(comp, instr->Node.Return);
				break;

			default:
				assert(!"Unknown type");
				break;
		}
		CHECK_STATUS;
	}

	return status;
}

static ProgramStatus CompileConstrNodes(x86Compiler* const comp)
{
	assert(comp);

	ProgramStatus status = ProgramStatus::Ok;

	const ExtArray* constrNodes = &comp->Ast->ConstrNodes;
	const size_t functCount = constrNodes->Size;

	for (size_t st = 0; st < functCount; st++)
	{
		ConstructionNode* node = (ConstructionNode*)ExtArrayGetElemAt(constrNodes, st);

		switch (node->Type)
		{
			case AstNodeTypes::FunctDef:
				status = CompileFunctDef(comp, node->Node.FunctCall);
				break;

			case AstNodeTypes::GlobVar:
				break;

			default:
				assert(!"Unknown");
				break;
		}
		CHECK_STATUS;
	}

	return status;
}

static ProgramStatus CompileInstrinsicFunction(x86Compiler* const comp, FunctCallNode* const functCallNode)
{
	assert(comp);
	assert(functCallNode);

	AsmRep* const asmRep = &comp->AsmRep;
	AsmArg arg1 = {};
	AsmArg arg2 = {};

	const Identifier* const id = IdentifierGetById(&comp->Ast->IdentififerTable, functCallNode->NameId);
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
		CHECK_STATUS;
		
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
		CHECK_STATUS;

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