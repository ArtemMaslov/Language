#include <assert.h>

#include "AST.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ExpressionNode* AstCreateExpressionNode(AST* ast, AstNodeTypes type, void* nodePtr);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ExpressionNode* AstCreateExpressionNode(AST* ast, AstNodeTypes type, void* nodePtr)
{
	assert(ast);
	assert(nodePtr);

	ExpressionNode* node = (ExpressionNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(ExpressionNode));

	if (!node)
		return nullptr;

	node->Type     = type;
	node->Node.Ptr = nodePtr;

	return node;
}
static ExpressionNode* AstCreateExpressionNode(AST* ast, AstNodeTypes type, double number)
{
	assert(ast);

	ExpressionNode* node = (ExpressionNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(ExpressionNode));

	if (!node)
		return nullptr;

	node->Type     = type;
	node->Node.Number = number;

	return node;
}

ExpressionNode* AstCreateExpressionNode(AST* ast, double number)
{
	return AstCreateExpressionNode(ast, AstNodeTypes::Number, number);
}

ExpressionNode* AstCreateExpressionNode(AST* ast, BinaryOperatorNode* binOper)
{
	return AstCreateExpressionNode(ast, AstNodeTypes::BinaryOperator, binOper);
}

ExpressionNode* AstCreateExpressionNode(AST* ast, UnaryOperatorNode* unOperNode)
{
	return AstCreateExpressionNode(ast, AstNodeTypes::UnaryOperator, unOperNode);
}

ExpressionNode* AstCreateExpressionNode(AST* ast, FunctCallNode* functCallNode)
{
	return AstCreateExpressionNode(ast, AstNodeTypes::FunctCall, functCallNode);
}

ExpressionNode* AstCreateExpressionNode(AST* ast, VariableNode* varNode)
{
	return AstCreateExpressionNode(ast, AstNodeTypes::Variable, varNode);
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

UnaryOperatorNode* AstCreateUnaryOperatorNode(AST* ast, OperatorType oper, ExpressionNode* operand)
{
	assert(ast);
	assert(operand);

	UnaryOperatorNode* node = (UnaryOperatorNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(UnaryOperatorNode));

	if (!node)
		return nullptr;

	node->Operator = oper;
	node->Operand  = operand;

	return node;
}

BinaryOperatorNode* AstCreateBinaryOperatorNode(AST* ast, OperatorType oper,
												ExpressionNode* leftOperand, ExpressionNode* rightOperand)
{
	assert(ast);
	assert(leftOperand);
	assert(rightOperand);

	BinaryOperatorNode* node = (BinaryOperatorNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(BinaryOperatorNode));

	if (!node)
		return nullptr;

	node->Operator     = oper;
	node->LeftOperand  = leftOperand;
	node->RightOperand = rightOperand;

	return node;
}

VariableNode* AstCreateVariableNode(AST* ast, const int nameId, ExpressionNode* initValue)
{
	assert(ast);
	//assert(initValue);

	VariableNode* node = (VariableNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(VariableNode));

	if (!node)
		return nullptr;

	node->NameId    = nameId;
	node->InitValue = initValue;

	return node;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 