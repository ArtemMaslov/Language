///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
//
// Второстепенные функции.
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

ProgramStatus CompileInstrBlock(x86Compiler* const comp, const ExtArray* const instrBlock)
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

ProgramStatus CompileConstrNodes(x86Compiler* const comp)
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

ProgramStatus IdentifierGetMemoryOffset(x86Compiler* const comp, const size_t varId, 
                                        int* const outOffset, bool* const outVarInitialization)
{
	assert(comp);
    assert(outOffset);
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

			*outOffset = 4 * (2 + (int)st);
            return ProgramStatus::Ok;
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

			*outOffset = ident->CompileMemoryOffset;
            return ProgramStatus::Ok;
		}
	}

	// Переменная раньше не встречалась. Создаём новую запись и выделяем память.

	const int offset = -4 * (int)(++table->CompileIdentInited);

	Identifier newIdent          = {};
	newIdent.CompileMemoryOffset = offset;
	newIdent.Id                  = varId;

	ProgramStatus status = ExtArrayAddElem(&table->Records, &newIdent);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return status;
	}

	if (outVarInitialization)
		*outVarInitialization = true;

	*outOffset = offset;
    return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///