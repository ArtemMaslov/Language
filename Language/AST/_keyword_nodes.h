#ifndef KEYWORD_NODES_H
#define KEYWORD_NODES_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

struct IfNode
{
	ExpressionNode* Condition;
	ExtArray TrueBlock;
	ExtArray FalseBlock;
};

struct WhileNode
{
	ExpressionNode* Condition;
	ExtArray Body;
};

struct InputNode
{
	VariableNode*  Input;
};

struct OutputNode
{
	ExpressionNode* Output;
};

struct ReturnNode
{
	ExpressionNode* Value;
};

const size_t AST_IF_BODY_DEFAULT_SIZE      = 128;
const size_t AST_WHILE_BODY_DEFAULT_SIZE   = 128;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AstWhileNodeConstructor(WhileNode* node);

ProgramStatus AstWhileNodeDestructor(WhileNode* node);

ProgramStatus AstIfNodeConstructor(IfNode* node);

ProgramStatus AstIfNodeDestructor(IfNode* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

InputNode* AstCreateInputNode(AST* ast, VariableNode* varNode);

OutputNode* AstCreateOutputNode(AST* ast, ExpressionNode* exprNode);

IfNode* AstCreateIfNode(AST* ast, ExpressionNode* condition);

WhileNode* AstCreateWhileNode(AST* ast, ExpressionNode* condition);

ReturnNode* AstCreateReturnNode(AST* ast, ExpressionNode* exprNode);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !KEYWORD_NODES_H