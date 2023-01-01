#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SoftCpuCompiler.h"
#include "../../Front end/Lexer/_identifier.h"

#include "_compiler_config.h"
#include "_intrinsic_funct.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define CHECK_STATUS \
	assert(status == ProgramStatus::Ok)
	//if (status != ProgramStatus::Ok) \
	//	return status;

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

struct x86Compiler
{
	size_t LabelIndex;

	FILE* File;
	AST*  Ast;

	/// Узел функции, которая сейчас компилируется.
	const FunctDefNode* CurrentFunction;

	IdentifierTable CurrentVariablesScope;
};

const size_t IdentifierNull = (size_t)(-1);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ProgramStatus CompileFunctDef(x86Compiler* comp, const FunctDefNode* functNode);

static ProgramStatus CompileFunctCall(x86Compiler* comp, const FunctCallNode* functCallNode);

static ProgramStatus CompileInstrinsicFunction(x86Compiler* comp, FunctCallNode* functCallNode);

static ProgramStatus CompileWhile(x86Compiler* comp, const WhileNode* whileNode);

static ProgramStatus CompileIf(x86Compiler* comp, const IfNode* ifNode);

static ProgramStatus CompileOutput(x86Compiler* comp, const OutputNode* outputNode);

static ProgramStatus CompileInput(x86Compiler* comp, const InputNode* inputNode);

static ProgramStatus CompileExpression(x86Compiler* comp, const ExpressionNode* exprNode);

static ProgramStatus CompileUnaryOperator(x86Compiler* comp, const UnaryOperatorNode* unOperNode);

static ProgramStatus CompileBinaryOperator(x86Compiler* comp, const BinaryOperatorNode* binOperNode);

static int IdentifierGetMemoryOffset(x86Compiler* comp, const int varId, bool* outVarInitialization = nullptr);

static ProgramStatus CompileReturn(x86Compiler* comp, const ReturnNode* returnNode);

static ProgramStatus CompileNumber(x86Compiler* comp, double number);

static ProgramStatus CompileVariable(x86Compiler* comp, const VariableNode* varNode);

static ProgramStatus CompileInstrBlock(x86Compiler* comp, const ExtArray* instrBlock);

static ProgramStatus CompileConstrNodes(x86Compiler* comp);

static ProgramStatus CompileCompareOperator(x86Compiler* comp, const BinaryOperatorNode* binOperNode);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus SoftCpuCompileFile(AST* ast, const char* outFile)
{
	assert(ast);
	assert(outFile);

	FILE* file = fopen(outFile, "w");

	if (!file)
	{
		return ProgramStatus::Fault;
	}

	x86Compiler comp = {};
	comp.File     = file;
	comp.Ast      = ast;

	fputs("\tcall main\n\thlt\n\n", file);

	ProgramStatus status = CompileConstrNodes(&comp);

	fclose(file);

	return ProgramStatus::Ok;
}

static ProgramStatus CompileFunctDef(x86Compiler* comp, const FunctDefNode* functNode)
{
	assert(comp);
	assert(functNode);

	ProgramStatus status = ProgramStatus::Ok;

	comp->CurrentFunction = functNode;

	status = IdentifierTableConstructor(&comp->CurrentVariablesScope);
	CHECK_STATUS;

	const Identifier* const id = IdentifierGetById(&comp->Ast->IdentififerTable, functNode->NameId);
	if (!id)
	{
		return ProgramStatus::Fault;
	}
	const char* const functNodeName = id->Name;

	fprintf(comp->File, CompFunctDefStart, functNodeName);

	status = CompileInstrBlock(comp, &functNode->Body);
	CHECK_STATUS;

	fputs(CompFunctDefEnd, comp->File);

	status = IdentifierTableDestructor(&comp->CurrentVariablesScope);
	CHECK_STATUS;

	return status;
}

static ProgramStatus CompileFunctCall(x86Compiler* comp, const FunctCallNode* functCallNode)
{
	assert(comp);
	assert(functCallNode);

	ProgramStatus status = ProgramStatus::Ok;

	const ExtArray* const params = &functCallNode->Params;
	const size_t paramsCount = params->Size;

	for (int st = (int)(paramsCount - 1); st >= 0; st--)
	{
		FunctParamNode* param = (FunctParamNode*)ExtArrayGetElemAt(params, st);

		status = CompileExpression(comp, param->Value);
		CHECK_STATUS;

		fputs(CompFunctCallSaveParam, comp->File);
	}

	const Identifier* const id = IdentifierGetById(&comp->Ast->IdentififerTable, functCallNode->NameId);
	if (!id)
	{
		return ProgramStatus::Fault;
	}
	const char* const functNodeName = id->Name;

	fprintf(comp->File, CompFunctCall, functNodeName, paramsCount);

	return status;
}

static ProgramStatus CompileReturn(x86Compiler* comp, const ReturnNode* returnNode)
{
	assert(returnNode);

	ProgramStatus status = ProgramStatus::Ok;

	if (returnNode->Value)
	{
		status = CompileExpression(comp, returnNode->Value);
		CHECK_STATUS;
		fputs(CompRetValue, comp->File);
	}

	fputs(CompFunctDefEnd, comp->File);

	return ProgramStatus::Ok;
}

static ProgramStatus CompileWhile(x86Compiler* comp, const WhileNode* whileNode)
{
	assert(comp);
	assert(whileNode);

	ProgramStatus status = ProgramStatus::Ok;
	const size_t labelIndex = comp->LabelIndex++;

	fprintf(comp->File, CompWhileConditionLabelFormat, labelIndex);

	status = CompileExpression(comp, whileNode->Condition);
	CHECK_STATUS;

	fprintf(comp->File, CompWhileConditionCheckFormat, labelIndex);

	status = CompileInstrBlock(comp, &whileNode->Body);
	CHECK_STATUS;

	fprintf(comp->File, CompWhileEndFormat, labelIndex, labelIndex);

	return status;
}

static ProgramStatus CompileIf(x86Compiler* comp, const IfNode* ifNode)
{
	assert(comp);
	assert(ifNode);

	ProgramStatus status = ProgramStatus::Ok;
	const size_t labelIndex = comp->LabelIndex++;
		
	status = CompileExpression(comp, ifNode->Condition);
	CHECK_STATUS;

	fprintf(comp->File, CompIfConditionFormat, labelIndex);

	status = CompileInstrBlock(comp, &ifNode->TrueBlock);
	CHECK_STATUS;

	fprintf(comp->File, CompIfFalseLabelFormat, labelIndex, labelIndex);

	status = CompileInstrBlock(comp, &ifNode->FalseBlock);
	CHECK_STATUS;

	fprintf(comp->File, CompIfEndLabelFormat, labelIndex);

	return status;
}

static ProgramStatus CompileOutput(x86Compiler* comp, const OutputNode* outputNode)
{
	assert(comp);
	assert(outputNode);

	ProgramStatus status = CompileExpression(comp, outputNode->Output);
	CHECK_STATUS;

	fputs(CompOutput, comp->File);

	return status;
}

static ProgramStatus CompileInput(x86Compiler* comp, const InputNode* inputNode)
{
	assert(comp);
	assert(inputNode);

	size_t memOffset = IdentifierGetMemoryOffset(comp, inputNode->Input->NameId);

	if (memOffset == IdentifierNull)
	{
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	fprintf(comp->File, CompInput, memOffset);

	return ProgramStatus::Ok;
}

static ProgramStatus CompileExpression(x86Compiler* comp, const ExpressionNode* exprNode)
{
	assert(comp);
	assert(exprNode);

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

			fputs(CompGetRetValue, comp->File);

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

static ProgramStatus CompileUnaryOperator(x86Compiler* comp, const UnaryOperatorNode* unOperNode)
{
	assert(comp);
	assert(unOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	status = CompileExpression(comp, unOperNode->Operand);
	CHECK_STATUS;

	const size_t labelIndex = comp->LabelIndex;

	switch (unOperNode->Operator)
	{
		case OperatorType::Not:
			fprintf(comp->File, CompNot, labelIndex, labelIndex, labelIndex, labelIndex);
			comp->LabelIndex++;
			break;

		case OperatorType::Subtraction:
			fputs(CompUnSubFormat, comp->File);
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

static ProgramStatus CompileBinaryOperator(x86Compiler* comp, const BinaryOperatorNode* binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	status = CompileExpression(comp, binOperNode->LeftOperand);
	CHECK_STATUS;

	status = CompileExpression(comp, binOperNode->RightOperand);
	CHECK_STATUS;

	const size_t labelIndex = comp->LabelIndex;

	switch (binOperNode->Operator)
	{
		case OperatorType::Addition:
			fputs(CompAdd, comp->File);
			break;

		case OperatorType::Subtraction:
			fputs(CompSub, comp->File);
			break;

		case OperatorType::Multiplication:
			fputs(CompMul, comp->File);
			break;

		case OperatorType::Division:
			fputs(CompDiv, comp->File);
			break;

		case OperatorType::Equal:
		case OperatorType::NotEqual:
		case OperatorType::Greater:
		case OperatorType::GreaterEqual:
		case OperatorType::Less:
		case OperatorType::LessEqual:
			return CompileCompareOperator(comp, binOperNode);

		case OperatorType::Not:
			fprintf(comp->File, CompNot,
					labelIndex,
					labelIndex,
					labelIndex,
					labelIndex);
			comp->LabelIndex++;
			break;

		case OperatorType::And:
			fprintf(comp->File, CompAnd,
					labelIndex,
					labelIndex,
					labelIndex,
					labelIndex,
					labelIndex);
			comp->LabelIndex++;
			break;

		case OperatorType::Or:
			fprintf(comp->File, CompOr, 
					labelIndex, 
					labelIndex, 
					labelIndex, 
					labelIndex, 
					labelIndex, 
					labelIndex);
			comp->LabelIndex++;
			break;

		default:
			assert(!"Unknown");
			break;
	}

	return status;
}

static ProgramStatus CompileCompareOperator(x86Compiler* comp, const BinaryOperatorNode* binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	const char*  jmpType    = nullptr;
	const char*  labelName  = nullptr;
	const size_t labelIndex = comp->LabelIndex;

	switch (binOperNode->Operator)
	{
		case OperatorType::Equal:
			jmpType   = CompEqualJmpType;
			labelName = CompEqualLabelName;
			break;

		case OperatorType::NotEqual:
			jmpType   = CompNotEqualJmpType;
			labelName = CompNotEqualLabelName;
			break;

		case OperatorType::Greater:
			jmpType   = CompGreaterJmpType;
			labelName = CompGreaterLabelName;
			break;

		case OperatorType::GreaterEqual:
			jmpType   = CompGreaterEqualJmpType;
			labelName = CompGreaterEqualLabelName;
			break;

		case OperatorType::Less:
			jmpType   = CompLessJmpType;
			labelName = CompLessLabelName;
			break;

		case OperatorType::LessEqual:
			jmpType   = CompLessEqualJmpType;
			labelName = CompLessEqualLabelName;
			break;

		default:
			assert(!"Error");
			break;
	}

	fprintf(comp->File, CompCompare,
			jmpType,
			labelName, labelIndex,
			labelName, labelIndex,
			labelName, labelIndex,
			labelName, labelIndex);

	comp->LabelIndex++;

	return status;
}

static int IdentifierGetMemoryOffset(x86Compiler* comp, const int varId, bool* outVarInitialization)
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

			return -1 - (int)st;
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

	const int offset = (int)(table->CompileIdentInited++);

	Identifier newIdent          = {};
	newIdent.CompileMemoryOffset = offset;
	newIdent.Id                  = varId;

	ProgramStatus status = ExtArrayAddElem(&table->Table, &newIdent);
	CHECK_STATUS;

	if (outVarInitialization)
		*outVarInitialization = true;

	return offset;
}

static ProgramStatus CompileNumber(x86Compiler* comp, double number)
{
	assert(comp);

	fprintf(comp->File, CompNumber, number);

	return ProgramStatus::Ok;
}

static ProgramStatus CompileVariable(x86Compiler* comp, const VariableNode* varNode)
{
	assert(comp);
	assert(varNode);

	ProgramStatus status = ProgramStatus::Ok;

	bool varInit = false;

	const int offset = IdentifierGetMemoryOffset(comp, varNode->NameId, &varInit);

	if (varNode->InitValue)
	{
		status = CompileExpression(comp, varNode->InitValue);
		CHECK_STATUS;

		if (varInit)
			fprintf(comp->File, CompVarInit, offset);
		else
			fprintf(comp->File, CompVarSetValue, offset);
	}
	else
	{
		fprintf(comp->File, CompVarGetValue, offset);
	}

	return status;
}

static ProgramStatus CompileInstrBlock(x86Compiler* comp, const ExtArray* instrBlock)
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

static ProgramStatus CompileConstrNodes(x86Compiler* comp)
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

static ProgramStatus CompileInstrinsicFunction(x86Compiler* comp, FunctCallNode* functCallNode)
{
	assert(comp);
	assert(functCallNode);

	const Identifier* const id = IdentifierGetById(&comp->Ast->IdentififerTable, functCallNode->NameId);
	if (!id)
	{
		return ProgramStatus::Fault;
	}
	const char* const functNodeName = id->Name;

	if (strncmp(functNodeName, IntrinSqrt.Name, IntrinSqrt.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 1);

		FunctParamNode* arg = (FunctParamNode*)ExtArrayGetElemAt(&functCallNode->Params, 0);

		ProgramStatus status = CompileExpression(comp, arg->Value);
		CHECK_STATUS;

		fputs(CompSqrt, comp->File);

		return status;
	}
	else if (strncmp(functNodeName, IntrinSetRam.Name, IntrinSetRam.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 2);

		FunctParamNode* ramAddr = (FunctParamNode*)ExtArrayGetElemAt(&functCallNode->Params, 0);
		FunctParamNode* value   = (FunctParamNode*)ExtArrayGetElemAt(&functCallNode->Params, 1);

		ProgramStatus status = ProgramStatus::Ok;
		
		status = CompileExpression(comp, value->Value);
		CHECK_STATUS;

		status = CompileExpression(comp, ramAddr->Value);
		CHECK_STATUS;
		
		fputs(CompSetRam, comp->File);

		return status;
	}
	else if (strncmp(functNodeName, IntrinGetRam.Name, IntrinGetRam.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 1);

		FunctParamNode* ramAddr = (FunctParamNode*)ExtArrayGetElemAt(&functCallNode->Params, 0);

		ProgramStatus status = CompileExpression(comp, ramAddr->Value);
		CHECK_STATUS;

		fputs(CompGetRam, comp->File);

		return status;
	}
	else if (strncmp(functNodeName, IntrinDisplay.Name, IntrinDisplay.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 0);

		fputs(CompDisplay, comp->File);

		return ProgramStatus::Ok;
	}
	else if (strncmp(functNodeName, IntrinInt.Name, IntrinInt.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 1);

		FunctParamNode* value = (FunctParamNode*)ExtArrayGetElemAt(&functCallNode->Params, 0);

		ProgramStatus status = CompileExpression(comp, value->Value);
		CHECK_STATUS;

		fputs(CompInt, comp->File);

		return ProgramStatus::Ok;
	}
	else
	{
		return CompileFunctCall(comp, functCallNode);
	}
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 