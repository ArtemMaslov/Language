///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
// 
// Функции поддержки встроенных в язык операторов.
//  
// Версия: 1.0.1.0
// Дата последнего изменения: 18:40 03.02.2023
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

AstError AstWhileNodeConstructor(WhileNode* const node)
{
	assert(node);

	ProgramStatus status = ExtArrayConstructor(&node->Body, sizeof(InstructionNode), AstWhileBodyDefaultCapacity);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return AstError::ExtArray;
	}

	return AstError::NoErrors;
}

void AstWhileNodeDestructor(WhileNode* const node)
{
	assert(node);
	
	AstInstrBlockDestructor(&node->Body);

	ExtArrayDestructor(&node->Body);
	memset(node, 0, sizeof(WhileNode));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

AstError AstIfNodeConstructor(IfNode* const node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;
	status = ExtArrayConstructor(&node->TrueBlock, sizeof(InstructionNode), AstIfBodyDefaultCapacity);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return AstError::ExtArray;
	}

	status = ExtArrayConstructor(&node->FalseBlock, sizeof(InstructionNode), AstIfBodyDefaultCapacity);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		ExtArrayDestructor(&node->TrueBlock);
		return AstError::ExtArray;
	}

	return AstError::NoErrors;
}

void AstIfNodeDestructor(IfNode* const node)
{
	assert(node);

	AstInstrBlockDestructor(&node->TrueBlock);
	AstInstrBlockDestructor(&node->FalseBlock);

	ExtArrayDestructor(&node->FalseBlock);
	ExtArrayDestructor(&node->TrueBlock);
	memset(node, 0, sizeof(IfNode));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

InputNode* AstCreateInputNode(AST* const ast, VariableNode* const varNode)
{
	assert(ast);
	assert(varNode);

	InputNode* node = (InputNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(InputNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Input = varNode;

	return node;
}

OutputNode* AstCreateOutputNode(AST* const ast, ExpressionNode* const exprNode)
{
	assert(ast);
	assert(exprNode);

	OutputNode* node = (OutputNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(OutputNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Output = exprNode;

	return node;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

IfNode* AstCreateIfNode(AST* const ast, ExpressionNode* const condition)
{
	assert(ast);
	assert(condition);

	IfNode* node = (IfNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(IfNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	if (AstIfNodeConstructor(node) != AstError::NoErrors)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Condition = condition;

	return node;
}

WhileNode* AstCreateWhileNode(AST* const ast, ExpressionNode* const condition)
{
	assert(ast);
	assert(condition);

	WhileNode* node = (WhileNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(WhileNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	if (AstWhileNodeConstructor(node) != AstError::NoErrors)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Condition = condition;

	return node;
}

ReturnNode* AstCreateReturnNode(AST* const ast, ExpressionNode* const exprNode)
{
	assert(ast);
	// assert(exprNode);

	ReturnNode* node = (ReturnNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(ReturnNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Value = exprNode;

	return node;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///