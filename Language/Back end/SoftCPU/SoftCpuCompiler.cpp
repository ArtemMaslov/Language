///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора для архитектуры эмулятора процессора SoftCpu.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 16:50 15.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../../Modules/Logs/Logs.h"
#include "../../Modules/ErrorsHandling.h"

#include "SoftCpuCompiler.h"
#include "../../Front end/Lexer/Identifier.h"

#include "_compiler_config.h"
#include "_intrinsic_funct.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Структура компилятора SoftCpu.
struct SoftCpuCompiler
{
	/// Индекс метки. Используется, чтобы у меток были уникальные имена.
	size_t LabelIndex;

	/// Указатель на выходной файл с ассемблерным кодом.
	FILE* File;
	/// Указатель на АСД.
	AST*  Ast;

	/// Узел функции, которая сейчас компилируется.
	const FunctDefNode* CurrentFunction;

	/// Текущая область видимости переменных. Используется для выделения переменным места
	/// в стеке сразу после вызова процедуры.
	IdentifierTable CurrentVariablesScope;
};

/// Значение нулевой (пустой) переменной. Сигнализирует об ошибке внутри вызванной функции.
const int IdentifierNull = INT32_MAX;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Скомпилировать определение функции.
 * 
 * @param comp      Указатель на структуру компилятора.
 * @param functNode Указатель на узел определения функции.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileFunctDef(SoftCpuCompiler* comp, const FunctDefNode* functNode);

/**
 * @brief  Скомпилировать вызов функции.
 * 
 * @param comp          Указатель на структуру компилятора.
 * @param functCallNode Указатель на узел вызова функции.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileFunctCall(SoftCpuCompiler* comp, const FunctCallNode* functCallNode);

/**
 * @brief  Скомпилировать встроенные в язык функции.
 * 
 * @param comp          Указатель на структуру компилятора.
 * @param functCallNode Указатель на узел вызова функции.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileInstrinsicFunction(SoftCpuCompiler* comp, FunctCallNode* functCallNode);

/**
 * @brief  Скомпилировать оператор цикла.
 * 
 * @param comp      Указатель на структуру компилятора.
 * @param whileNode Указатель на узел оператора цикла.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileWhile(SoftCpuCompiler* comp, const WhileNode* whileNode);

/**
 * @brief  Скомпилировать условный оператор.
 * 
 * @param comp   Указатель на структуру компилятора.
 * @param ifNode Указатель на узел условного оператора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileIf(SoftCpuCompiler* comp, const IfNode* ifNode);

/**
 * @brief  Скомпилировать оператор вывода.
 * 
 * @param comp       Указатель на структуру компилятора.
 * @param outputNode Указатель на узел оператора вывода.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileOutput(SoftCpuCompiler* comp, const OutputNode* outputNode);

/**
 * @brief  Скомпилировать оператор ввода.
 * 
 * @param comp      Указатель на структуру компилятора.
 * @param inputNode Указатель на оператор ввода.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileInput(SoftCpuCompiler* comp, const InputNode* inputNode);

/**
 * @brief  Скомпилировать выражение.
 * 
 * @param comp     Указатель на структуру компилятора.
 * @param exprNode Указатель на узел выражения.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileExpression(SoftCpuCompiler* comp, const ExpressionNode* exprNode);

/**
 * @brief  Скомпилировать унарный оператор.
 * 
 * @param comp       Указатель на структуру компилятора.
 * @param unOperNode Указатель на узел унарного оператора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileUnaryOperator(SoftCpuCompiler* comp, const UnaryOperatorNode* unOperNode);

/**
 * @brief  Скомпилировать бинарный оператор.
 * 
 * @param comp        Указатель на структуру компилятора.
 * @param binOperNode Указатель на узел бинарного оператора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileBinaryOperator(SoftCpuCompiler* comp, const BinaryOperatorNode* binOperNode);

/**
 * @brief  Скомпилировать оператор возврата.
 * 
 * @param comp       Указатель на структуру компилятора.
 * @param returnNode Указатель на узел оператора возврата.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileReturn(SoftCpuCompiler* comp, const ReturnNode* returnNode);

/**
 * @brief  Скомпилировать использование константного числа с плавающей точкой.
 * 
 * @param comp   Указатель на структуру компилятора.
 * @param number Значение числа с плавающей точкой.
 */
static void CompileNumber(SoftCpuCompiler* comp, double number);

/**
 * @brief  Скомпилировать использование переменной (инициализация и получение значения).
 * 
 * @param comp    Указатель на структуру компилятора.
 * @param varNode Указатель на узел переменной.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileVariable(SoftCpuCompiler* comp, const VariableNode* varNode);

/**
 * @brief  Скомпилировать блок инструкций.
 * 
 * @param comp       Указатель на структуру компилятора.
 * @param instrBlock ExtArray<InstructionNode> - блок инструкций.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileInstrBlock(SoftCpuCompiler* comp, const ExtArray* instrBlock);

/**
 * @brief  Скомпилировать конструкционные узлы.
 * 
 * @param comp Указатель на структуру компилятора.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static ProgramStatus CompileConstrNodes(SoftCpuCompiler* comp);

/**
 * @brief  Скомпилировать оператор сравнения.
 * 
 * @param comp        Указатель на структуру компилятора.
 * @param binOperNode Указатель на бинарный оператор, который является оператором сравнения.
 */
static void CompileCompareOperator(SoftCpuCompiler* comp, const BinaryOperatorNode* binOperNode);

/**
 * @brief  Получить смещение в стеке для адреса локальной переменной функции.
 * 
 * @param comp                 Указатель на структуру компилятора.
 * @param varId                Идентификатор переменной.
 * @param outVarInitialization Выходной параметр. Равен true, если переменная раньше 
 *                             не встречалась в функции, то есть происходит её инициализация.
 *                             false, если происходит получение значения переменной.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
static int IdentifierGetMemoryOffset(SoftCpuCompiler* comp, const size_t varId, bool* outVarInitialization = nullptr);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus SoftCpuCompileFile(AST* ast, const char* outFile)
{
	assert(ast);
	assert(outFile);

	ProgramStatus status = ProgramStatus::Ok;

	FILE* const file = fopen(outFile, "w");
	if (!file)
	{
		LOG_F_ERR("Ошибка открытия файла \"%s\"", file);
		return ProgramStatus::Fault;
	}

	SoftCpuCompiler comp = {};
	comp.File            = file;
	comp.Ast             = ast;

	fputs("\tcall main\n\thlt\n\n", file);

	status = CompileConstrNodes(&comp);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		fclose(file);
		return status;
	}
	
	fclose(file);

	return status;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static ProgramStatus CompileFunctDef(SoftCpuCompiler* comp, const FunctDefNode* functNode)
{
	assert(comp);
	assert(functNode);

	ProgramStatus status = ProgramStatus::Ok;

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

	fprintf(comp->File, CompFunctDefStart, functNodeName);

	status = CompileInstrBlock(comp, &functNode->Body);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		IdentifierTableDestructor(&comp->CurrentVariablesScope);
		return ProgramStatus::Fault;
	}

	fputs(CompFunctDefEnd, comp->File);

	IdentifierTableDestructor(&comp->CurrentVariablesScope);

	return status;
}

static ProgramStatus CompileFunctCall(SoftCpuCompiler* comp, const FunctCallNode* functCallNode)
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
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

		fputs(CompFunctCallSaveParam, comp->File);
	}

	const Identifier* const id = IdentifierGetById(comp->Ast->Identifiers, functCallNode->NameId);
	if (!id)
	{
		TRACE_ERROR();
		return ProgramStatus::Fault;
	}

	const char* const functNodeName = id->Name;

	fprintf(comp->File, CompFunctCall, functNodeName, paramsCount);

	return status;
}

static ProgramStatus CompileReturn(SoftCpuCompiler* comp, const ReturnNode* returnNode)
{
	assert(returnNode);

	ProgramStatus status = ProgramStatus::Ok;

	if (returnNode->Value)
	{
		status = CompileExpression(comp, returnNode->Value);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

		fputs(CompRetValue, comp->File);
	}

	fputs(CompFunctDefEnd, comp->File);

	return status;
}

static ProgramStatus CompileWhile(SoftCpuCompiler* comp, const WhileNode* whileNode)
{
	assert(comp);
	assert(whileNode);

	ProgramStatus status = ProgramStatus::Ok;
	const size_t labelIndex = comp->LabelIndex++;

	fprintf(comp->File, CompWhileConditionLabelFormat, labelIndex);

	status = CompileExpression(comp, whileNode->Condition);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	fprintf(comp->File, CompWhileConditionCheckFormat, labelIndex);

	status = CompileInstrBlock(comp, &whileNode->Body);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	fprintf(comp->File, CompWhileEndFormat, labelIndex, labelIndex);

	return status;
}

static ProgramStatus CompileIf(SoftCpuCompiler* comp, const IfNode* ifNode)
{
	assert(comp);
	assert(ifNode);

	ProgramStatus status = ProgramStatus::Ok;
	const size_t labelIndex = comp->LabelIndex++;
		
	status = CompileExpression(comp, ifNode->Condition);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	fprintf(comp->File, CompIfConditionFormat, labelIndex);

	status = CompileInstrBlock(comp, &ifNode->TrueBlock);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	fprintf(comp->File, CompIfFalseLabelFormat, labelIndex, labelIndex);

	status = CompileInstrBlock(comp, &ifNode->FalseBlock);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	fprintf(comp->File, CompIfEndLabelFormat, labelIndex);

	return status;
}

static ProgramStatus CompileOutput(SoftCpuCompiler* comp, const OutputNode* outputNode)
{
	assert(comp);
	assert(outputNode);

	ProgramStatus status = CompileExpression(comp, outputNode->Output);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	fputs(CompOutput, comp->File);

	return status;
}

static ProgramStatus CompileInput(SoftCpuCompiler* comp, const InputNode* inputNode)
{
	assert(comp);
	assert(inputNode);

	const int offset = IdentifierGetMemoryOffset(comp, inputNode->Input->NameId);
	if (offset == IdentifierNull)
	{
		TRACE_ERROR();
		return ProgramStatus::Fault;
	}

	fprintf(comp->File, CompInput, offset);

	return ProgramStatus::Ok;
}

static ProgramStatus CompileExpression(SoftCpuCompiler* comp, const ExpressionNode* exprNode)
{
	assert(comp);
	assert(exprNode);

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
			CompileNumber(comp, exprNode->Node.Number);
			break;

		case AstNodeType::FunctCall:
			status = CompileInstrinsicFunction(comp, exprNode->Node.FunctCall);

			fputs(CompGetRetValue, comp->File);

			break;

		case AstNodeType::Variable:
			status = CompileVariable(comp, exprNode->Node.Variable);
			break;

		default:
			assert(!"Unknown");
			break;
	}

	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	return status;
}

static ProgramStatus CompileUnaryOperator(SoftCpuCompiler* comp, const UnaryOperatorNode* unOperNode)
{
	assert(comp);
	assert(unOperNode);

	ProgramStatus status = ProgramStatus::Ok;

	status = CompileExpression(comp, unOperNode->Operand);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}
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

static ProgramStatus CompileBinaryOperator(SoftCpuCompiler* comp, const BinaryOperatorNode* binOperNode)
{
	assert(comp);
	assert(binOperNode);

	ProgramStatus status = ProgramStatus::Ok;

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
			CompileCompareOperator(comp, binOperNode);
			break;

		// case OperatorType::Not:
		// 	fprintf(comp->File, CompNot,
		// 			labelIndex,
		// 			labelIndex,
		// 			labelIndex,
		// 			labelIndex);
		// 	comp->LabelIndex++;
		// 	break;

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

static void CompileCompareOperator(SoftCpuCompiler* comp, const BinaryOperatorNode* binOperNode)
{
	assert(comp);
	assert(binOperNode);

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
}

static int IdentifierGetMemoryOffset(SoftCpuCompiler* comp, const size_t varId, bool* outVarInitialization)
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
	const ExtArray* const identifiers = &table->Records;
	const size_t identifiersCount = table->Records.Size;

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

	ProgramStatus status = ExtArrayAddElem(&table->Records, &newIdent);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return IdentifierNull;
	}

	if (outVarInitialization)
		*outVarInitialization = true;

	return offset;
}

static void CompileNumber(SoftCpuCompiler* comp, double number)
{
	assert(comp);

	fprintf(comp->File, CompNumber, number);
}

static ProgramStatus CompileVariable(SoftCpuCompiler* comp, const VariableNode* varNode)
{
	assert(comp);
	assert(varNode);

	ProgramStatus status = ProgramStatus::Ok;

	bool varInit = false;

	const int offset = IdentifierGetMemoryOffset(comp, varNode->NameId, &varInit);
	if (offset == IdentifierNull)
	{
		TRACE_ERROR();
		return ProgramStatus::Fault;
	}

	if (varNode->InitValue)
	{
		status = CompileExpression(comp, varNode->InitValue);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

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

static ProgramStatus CompileInstrBlock(SoftCpuCompiler* comp, const ExtArray* instrBlock)
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
			case AstNodeType::Variable:
				status = CompileVariable(comp, instr->Node.Variable);
				break;

			case AstNodeType::FunctCall:
				status = CompileInstrinsicFunction(comp, instr->Node.FunctCall);
				break;

			case AstNodeType::If:
				status = CompileIf(comp, instr->Node.If);
				break;

			case AstNodeType::While:
				status = CompileWhile(comp, instr->Node.While);
				break;

			case AstNodeType::Input:
				status = CompileInput(comp, instr->Node.Input);
				break;

			case AstNodeType::Output:
				status = CompileOutput(comp, instr->Node.Output);
				break;

			case AstNodeType::Return:
				status = CompileReturn(comp, instr->Node.Return);
				break;

			default:
				assert(!"Unknown type");
				break;
		}

		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}
	}

	return status;
}

static ProgramStatus CompileConstrNodes(SoftCpuCompiler* comp)
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
			case AstNodeType::FunctDef:
				status = CompileFunctDef(comp, node->Node.FunctDef);
				break;

			case AstNodeType::GlobVar:
				break;

			default:
				assert(!"Unknown");
				break;
		}
		
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}
	}

	return status;
}

static ProgramStatus CompileInstrinsicFunction(SoftCpuCompiler* comp, FunctCallNode* functCallNode)
{
	assert(comp);
	assert(functCallNode);

	const Identifier* const id = IdentifierGetById(comp->Ast->Identifiers, functCallNode->NameId);
	if (!id)
	{
		TRACE_ERROR();
		return ProgramStatus::Fault;
	}

	const char* const functNodeName = id->Name;

	if (strncmp(functNodeName, IntrinSqrt.Name, IntrinSqrt.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 1);

		FunctParamNode* arg = (FunctParamNode*)ExtArrayGetElemAt(&functCallNode->Params, 0);

		ProgramStatus status = CompileExpression(comp, arg->Value);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

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
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

		status = CompileExpression(comp, ramAddr->Value);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}
		
		fputs(CompSetRam, comp->File);

		return status;
	}
	else if (strncmp(functNodeName, IntrinGetRam.Name, IntrinGetRam.NameSize) == 0)
	{
		assert(functCallNode->Params.Size == 1);

		FunctParamNode* ramAddr = (FunctParamNode*)ExtArrayGetElemAt(&functCallNode->Params, 0);

		ProgramStatus status = CompileExpression(comp, ramAddr->Value);
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

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
		if (status != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return status;
		}

		fputs(CompInt, comp->File);

		return ProgramStatus::Ok;
	}
	else
	{
		return CompileFunctCall(comp, functCallNode);
	}
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///