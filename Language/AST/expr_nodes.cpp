///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
// 
// Функции работы с узлами выражений.
//  
// Версия: 1.0.1.0
// Дата последнего изменения: 16:03 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>

#include "../Modules/Logs/Logs.h"
#include "../Modules/ErrorsHandling.h"

#include "AST.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Создать узел выражения.
 * 
 * @param ast     Указатель на АСД.
 * @param type    Тип узла, оборачиваемого в выражение.
 * @param nodePtr Указатель на содержимое оборачиваемого узла.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
static ExpressionNode* AstExpressionNodeConstructor(AST* const ast, const AstNodeType type, void* const nodePtr);

/**
 * @brief  Создать числовой узел.
 * 
 * @param ast    Указатель на АСД.
 * @param number Число.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
static ExpressionNode* AstExpressionNodeConstructor(AST* const ast, const double number);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static ExpressionNode* AstExpressionNodeConstructor(AST* const ast, const AstNodeType type, void* const nodePtr)
{
	assert(ast);
	assert(nodePtr);

	ExpressionNode* const node = (ExpressionNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(ExpressionNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Type     = type;
	node->Node.Ptr = nodePtr;

	return node;
}

static ExpressionNode* AstExpressionNodeConstructor(AST* const ast, const double number)
{
	assert(ast);

	ExpressionNode* const node = (ExpressionNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(ExpressionNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Type        = AstNodeType::Number;
	node->Node.Number = number;

	return node;
}

ExpressionNode* AstCreateExpressionNode(AST* const ast, const double number)
{
	return AstExpressionNodeConstructor(ast, number);
}

ExpressionNode* AstCreateExpressionNode(AST* const ast, BinaryOperatorNode* const binOper)
{
	return AstExpressionNodeConstructor(ast, AstNodeType::BinaryOperator, binOper);
}

ExpressionNode* AstCreateExpressionNode(AST* const ast, UnaryOperatorNode* const unOperNode)
{
	return AstExpressionNodeConstructor(ast, AstNodeType::UnaryOperator, unOperNode);
}

ExpressionNode* AstCreateExpressionNode(AST* const ast, FunctCallNode* const functCallNode)
{
	return AstExpressionNodeConstructor(ast, AstNodeType::FunctCall, functCallNode);
}

ExpressionNode* AstCreateExpressionNode(AST* const ast, VariableNode* const varNode)
{
	return AstExpressionNodeConstructor(ast, AstNodeType::Variable, varNode);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

UnaryOperatorNode* AstCreateUnaryOperatorNode(AST* const ast, const OperatorType oper, ExpressionNode* const operand)
{
	assert(ast);
	assert(operand);

	UnaryOperatorNode* const node = (UnaryOperatorNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(UnaryOperatorNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Operator = oper;
	node->Operand  = operand;

	return node;
}

BinaryOperatorNode* AstCreateBinaryOperatorNode(AST* const ast, const OperatorType oper,
												ExpressionNode* const leftOperand, ExpressionNode* const rightOperand)
{
	assert(ast);
	assert(leftOperand);
	assert(rightOperand);

	BinaryOperatorNode* const node = (BinaryOperatorNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(BinaryOperatorNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Operator     = oper;
	node->LeftOperand  = leftOperand;
	node->RightOperand = rightOperand;

	return node;
}

VariableNode* AstCreateVariableNode(AST* const ast, const size_t nameId, ExpressionNode* const initValue)
{
	assert(ast);
	//assert(initValue);

	VariableNode* const node = (VariableNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(VariableNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->NameId    = nameId;
	node->InitValue = initValue;

	return node;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///