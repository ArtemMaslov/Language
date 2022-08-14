#include <assert.h>

#include "AST.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define CHECK_STATUS \
	if (status != ProgramStatus::Ok) \
		return status

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus AstFunctDefNodeConstructor(FunctDefNode* node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayConstructor(&node->Body, sizeof(InstructionNode), AST_FUNCT_BODY_DEFAULT_SIZE);
	CHECK_STATUS;

	status = ExtArrayConstructor(&node->Params, sizeof(FunctParamNode), AST_FUNCT_PARAMS_DEFAULT_SIZE);

	return status;
}

ProgramStatus AstFunctDefNodeDestructor(FunctDefNode* node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayDestructor(&node->Body);
	CHECK_STATUS;

	status = ExtArrayDestructor(&node->Params);

	return status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus AstFunctCallNodeConstructor(FunctCallNode* node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayConstructor(&node->Params, sizeof(FunctParamNode), AST_FUNCT_PARAMS_DEFAULT_SIZE);

	return status;
}

ProgramStatus AstFunctCallNodeDestructor(FunctCallNode* node)
{
	assert(node);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayDestructor(&node->Params);

	return status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

FunctDefNode* AstCreateFunctDefNode(AST* ast, int nameId)
{
	assert(ast);

	FunctDefNode* node = (FunctDefNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(FunctDefNode));

	if (!node)
		return nullptr;

	if (AstFunctDefNodeConstructor(node) != ProgramStatus::Ok)
		return nullptr;

	node->NameId = nameId;

	return node;
}

FunctCallNode* AstCreateFunctCallNode(AST* ast, int nameId)
{
	assert(ast);

	FunctCallNode* node = (FunctCallNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(FunctCallNode));

	if (!node)
		return nullptr;

	if (AstFunctCallNodeConstructor(node) != ProgramStatus::Ok)
		return nullptr;

	node->NameId = nameId;

	return node;
}

FunctParamNode* AstCreateFunctParamNode(AST* ast, ExpressionNode* expr)
{
	assert(ast);

	FunctParamNode* node = (FunctParamNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(FunctParamNode));

	if (!node)
		return nullptr;

	node->Value = expr;

	return node;
}

FunctParamNode* AstCreateFunctParamNode(AST* ast, int nameId)
{
	assert(ast);

	FunctParamNode* node = (FunctParamNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(FunctParamNode));

	if (!node)
		return nullptr;

	node->NameId = nameId;

	return node;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 