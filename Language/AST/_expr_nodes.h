#ifndef EXPRESSION_NODES_H
#define EXPRESSION_NODES_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

struct VariableNode
{
	int NameId;
	ExpressionNode* InitValue;
};

struct BinaryOperatorNode
{
	OperatorType Operator;

	ExpressionNode* LeftOperand;
	ExpressionNode* RightOperand;
};

struct UnaryOperatorNode
{
	OperatorType Operator;

	ExpressionNode* Operand;
};

struct ExpressionNode
{
	AstNodeTypes Type;

	union
	{
		void*               Ptr;
		BinaryOperatorNode* BinaryOperator;
		UnaryOperatorNode*  UnaryOperator;
		double              Number;
		FunctCallNode*      FunctCall;
		VariableNode*       Variable;
	} Node;
};

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ExpressionNode* AstCreateExpressionNode(AST* ast, double number);

ExpressionNode* AstCreateExpressionNode(AST* ast, BinaryOperatorNode* binOper);

ExpressionNode* AstCreateExpressionNode(AST* ast, UnaryOperatorNode* unOperNode);

ExpressionNode* AstCreateExpressionNode(AST* ast, FunctCallNode* functCallNode);

ExpressionNode* AstCreateExpressionNode(AST* ast, VariableNode* varNode);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

UnaryOperatorNode* AstCreateUnaryOperatorNode(AST* ast, OperatorType oper, ExpressionNode* operand);

BinaryOperatorNode* AstCreateBinaryOperatorNode(AST* ast, OperatorType oper,
												ExpressionNode* leftOperand, ExpressionNode* rightOperand);

VariableNode* AstCreateVariableNode(AST* ast, const int nameId, ExpressionNode* initValue);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !EXPRESSION_NODES_H