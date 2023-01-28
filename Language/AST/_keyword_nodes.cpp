#include <assert.h>

#include "AST.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define CHECK_STATUS \
	if (status != ProgramStatus::Ok) \
		return status

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AstWhileNodeConstructor(WhileNode* node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayConstructor(&node->Body, sizeof(InstructionNode), AST_WHILE_BODY_DEFAULT_SIZE);

	return status;
}

ProgramStatus AstWhileNodeDestructor(WhileNode* node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;

	ExtArrayDestructor(&node->Body);

	return status;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AstIfNodeConstructor(IfNode* node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayConstructor(&node->TrueBlock, sizeof(InstructionNode), AST_IF_BODY_DEFAULT_SIZE);
	CHECK_STATUS;

	status = ExtArrayConstructor(&node->FalseBlock, sizeof(InstructionNode), AST_IF_BODY_DEFAULT_SIZE);

	return status;
}

ProgramStatus AstIfNodeDestructor(IfNode* node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;

	ExtArrayDestructor(&node->TrueBlock);
	CHECK_STATUS;

	ExtArrayDestructor(&node->FalseBlock);

	return status;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

InputNode* AstCreateInputNode(AST* ast, VariableNode* varNode)
{
	assert(ast);
	assert(varNode);

	InputNode* node = (InputNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(InputNode));

	if (!node)
		return nullptr;

	node->Input = varNode;

	return node;
}

OutputNode* AstCreateOutputNode(AST* ast, ExpressionNode* exprNode)
{
	assert(ast);
	assert(exprNode);

	OutputNode* node = (OutputNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(OutputNode));

	if (!node)
		return nullptr;

	node->Output = exprNode;

	return node;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

IfNode* AstCreateIfNode(AST* ast, ExpressionNode* condition)
{
	assert(ast);
	assert(condition);

	IfNode* node = (IfNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(IfNode));

	if (!node)
		return nullptr;

	if (AstIfNodeConstructor(node) != ProgramStatus::Ok)
		return nullptr;

	node->Condition = condition;

	return node;
}

WhileNode* AstCreateWhileNode(AST* ast, ExpressionNode* condition)
{
	assert(ast);
	assert(condition);

	WhileNode* node = (WhileNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(WhileNode));

	if (!node)
		return nullptr;

	if (AstWhileNodeConstructor(node) != ProgramStatus::Ok)
		return nullptr;

	node->Condition = condition;

	return node;
}

ReturnNode* AstCreateReturnNode(AST* ast, ExpressionNode* exprNode)
{
	assert(ast);
	// assert(exprNode);

	ReturnNode* node = (ReturnNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(ReturnNode));

	if (!node)
		return nullptr;

	node->Value = exprNode;

	return node;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///