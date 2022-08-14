#include <assert.h>

#include "AST.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ConstructionNode* AstCreateConstrNode(AST* ast, AstNodeTypes type, void* nodePtr);

static InstructionNode* AstCreateInstructionNode(AST* ast, AstNodeTypes type, void* nodePtr);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ConstructionNode* AstCreateConstrNode(AST* ast, AstNodeTypes type, void* nodePtr)
{
	assert(ast);
	assert(nodePtr);

	ConstructionNode* node = (ConstructionNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(ConstructionNode));

	if (!node)
		return nullptr;

	node->Type     = type;
	node->Node.Ptr = nodePtr;

	return node;
}

ConstructionNode* AstCreateConstrNode(AST* ast, FunctDefNode* functNode)
{
	return AstCreateConstrNode(ast, AstNodeTypes::FunctDef, functNode);
}

ConstructionNode* AstCreateConstrNode(AST* ast, VariableNode* globVarNode)
{
	return AstCreateConstrNode(ast, AstNodeTypes::GlobVar, globVarNode);
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static InstructionNode* AstCreateInstructionNode(AST* ast, AstNodeTypes type, void* nodePtr)
{
	assert(ast);
	assert(nodePtr);

	InstructionNode* node = (InstructionNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(InstructionNode));

	if (!node)
		return nullptr;

	node->Type     = type;
	node->Node.Ptr = nodePtr;

	return node;
}

InstructionNode* AstCreateInstructionNode(AST* ast, VariableNode* varNode)
{
	return AstCreateInstructionNode(ast, AstNodeTypes::Variable, varNode);
}

InstructionNode* AstCreateInstructionNode(AST* ast, FunctCallNode* functCallNode)
{
	return AstCreateInstructionNode(ast, AstNodeTypes::FunctCall, functCallNode);
}

InstructionNode* AstCreateInstructionNode(AST* ast, IfNode* ifNode)
{
	return AstCreateInstructionNode(ast, AstNodeTypes::If, ifNode);
}

InstructionNode* AstCreateInstructionNode(AST* ast, WhileNode* whileNode)
{
	return AstCreateInstructionNode(ast, AstNodeTypes::While, whileNode);
}

InstructionNode* AstCreateInstructionNode(AST* ast, InputNode* inputNode)
{
	return AstCreateInstructionNode(ast, AstNodeTypes::Input, inputNode);
}

InstructionNode* AstCreateInstructionNode(AST* ast, OutputNode* outputNode)
{
	return AstCreateInstructionNode(ast, AstNodeTypes::Output, outputNode);
}

InstructionNode* AstCreateInstructionNode(AST* ast, ReturnNode* retNode)
{
	return AstCreateInstructionNode(ast, AstNodeTypes::Return, retNode);
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 