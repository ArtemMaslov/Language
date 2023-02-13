///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
// 
// Функции поддержки функциональной парадигмы языка.
//  
// Версия: 1.0.1.0
// Дата последнего изменения: 16:53 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <string.h>

#include "../Modules/Logs/Logs.h"
#include "../Modules/ErrorsHandling.h"

#include "AST.h"
#include "ast_private.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

AstError AstFunctDefNodeConstructor(FunctDefNode* const node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayConstructor(&node->Body, sizeof(InstructionNode), AstFunctBodyDefaultCapacity);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return AstError::ExtArray;
	}

	status = ExtArrayConstructor(&node->Params, sizeof(FunctParamNode), AstFunctParamsDefaultCapacity);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		ExtArrayDestructor(&node->Body);
		return AstError::ExtArray;
	}

	return AstError::NoErrors;
}

void AstFunctDefNodeDestructor(FunctDefNode* const node)
{
	assert(node);

	AstInstrBlockDestructor(&node->Body);
	
	ExtArrayDestructor(&node->Params);
	ExtArrayDestructor(&node->Body);
	memset(node, 0, sizeof(FunctDefNode));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

AstError AstFunctCallNodeConstructor(FunctCallNode* const node)
{
	assert(node);

	ProgramStatus status = ExtArrayConstructor(&node->Params, sizeof(FunctParamNode), AstFunctParamsDefaultCapacity);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return AstError::ExtArray;
	}

	return AstError::NoErrors;
}

void AstFunctCallNodeDestructor(FunctCallNode* const node)
{
	assert(node);

	ExtArrayDestructor(&node->Params);
	memset(node, 0, sizeof(FunctCallNode));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

FunctDefNode* AstCreateFunctDefNode(AST* const ast, const size_t nameId)
{
	assert(ast);

	FunctDefNode* const node = (FunctDefNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(FunctDefNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	if (AstFunctDefNodeConstructor(node) != AstError::NoErrors)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->NameId = nameId;

	return node;
}

FunctCallNode* AstCreateFunctCallNode(AST* const ast, const size_t nameId)
{
	assert(ast);

	FunctCallNode* const node = (FunctCallNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(FunctCallNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	if (AstFunctCallNodeConstructor(node) != AstError::NoErrors)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->NameId = nameId;

	return node;
}

FunctParamNode* AstCreateFunctParamNode(AST* const ast, ExpressionNode* const expr)
{
	assert(ast);

	FunctParamNode* const node = (FunctParamNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(FunctParamNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Value = expr;

	return node;
}

FunctParamNode* AstCreateFunctParamNode(AST* const ast, const size_t nameId)
{
	assert(ast);

	FunctParamNode* const node = (FunctParamNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(FunctParamNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->NameId = nameId;

	return node;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///