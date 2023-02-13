///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
// 
// Функции работы с служебными узлами АСД: строительными узлами и инструкциями.
//  
// Версия: 1.0.2.0
// Дата последнего изменения: 18:58 03.02.2023
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

/**
 * @brief  Создать строительный узел.
 * 
 * @param ast     Указатель на АСД.
 * @param type    Тип узла, оборачиваемого в строительный.
 * @param nodePtr Указатель на содержимое оборачиваемого узла.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
static ConstructionNode* AstCreateConstrNode(AST* ast, AstNodeType type, void* nodePtr);

/**
 * @brief  Создать узел инструкции.
 * 
 * @param ast     Указатель на АСД.
 * @param type    Тип узла, оборачиваемого в инструкцию.
 * @param nodePtr Указатель на содержимое оборачиваемого узла.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
static InstructionNode* AstCreateInstructionNode(AST* ast, AstNodeType type, void* nodePtr);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static ConstructionNode* AstCreateConstrNode(AST* const ast, const AstNodeType type, void* const nodePtr)
{
	assert(ast);
	assert(nodePtr);

	ConstructionNode* const node = (ConstructionNode* const)ExtHeapAllocElem(&ast->Nodes, sizeof(ConstructionNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Type     = type;
	node->Node.Ptr = nodePtr;

	return node;
}

ConstructionNode* AstCreateConstrNode(AST* const ast, FunctDefNode* const functNode)
{
	return AstCreateConstrNode(ast, AstNodeType::FunctDef, functNode);
}

ConstructionNode* AstCreateConstrNode(AST* const ast, VariableNode* const globVarNode)
{
	return AstCreateConstrNode(ast, AstNodeType::GlobVar, globVarNode);
}

void AstConstrNodeDestructor(ConstructionNode* const node)
{
	assert(node);

	if (node->Type == AstNodeType::FunctDef)
		AstFunctDefNodeDestructor(node->Node.FunctDef);
	memset(node, 0, sizeof(ConstructionNode));
}

void AstConstrBlockDestructor(ExtArray* const constrBlock)
{
	assert(constrBlock);

	const size_t nodesCount = constrBlock->Size;
	for (size_t st = 0; st < nodesCount; st++)
		AstConstrNodeDestructor((ConstructionNode*)ExtArrayGetElemAt(constrBlock, st));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static InstructionNode* AstCreateInstructionNode(AST* const ast, const AstNodeType type, void* const nodePtr)
{
	assert(ast);
	assert(nodePtr);

	InstructionNode* node = (InstructionNode*)ExtHeapAllocElem(&ast->Nodes, sizeof(InstructionNode));
	if (!node)
	{
		TRACE_ERROR();
		return nullptr;
	}

	node->Type     = type;
	node->Node.Ptr = nodePtr;

	return node;
}

InstructionNode* AstCreateInstructionNode(AST* const ast, VariableNode* const varNode)
{
	return AstCreateInstructionNode(ast, AstNodeType::Variable, varNode);
}

InstructionNode* AstCreateInstructionNode(AST* const ast, FunctCallNode* const functCallNode)
{
	return AstCreateInstructionNode(ast, AstNodeType::FunctCall, functCallNode);
}

InstructionNode* AstCreateInstructionNode(AST* const ast, IfNode* const ifNode)
{
	return AstCreateInstructionNode(ast, AstNodeType::If, ifNode);
}

InstructionNode* AstCreateInstructionNode(AST* const ast, WhileNode* const whileNode)
{
	return AstCreateInstructionNode(ast, AstNodeType::While, whileNode);
}

InstructionNode* AstCreateInstructionNode(AST* const ast, InputNode* const inputNode)
{
	return AstCreateInstructionNode(ast, AstNodeType::Input, inputNode);
}

InstructionNode* AstCreateInstructionNode(AST* const ast, OutputNode* const outputNode)
{
	return AstCreateInstructionNode(ast, AstNodeType::Output, outputNode);
}

InstructionNode* AstCreateInstructionNode(AST* const ast, ReturnNode* const retNode)
{
	return AstCreateInstructionNode(ast, AstNodeType::Return, retNode);
}

void AstInstrNodeDestructor(InstructionNode* const node)
{
	assert(node);

	switch (node->Type)
	{
		case AstNodeType::If:
			AstIfNodeDestructor(node->Node.If);
			break;
		case AstNodeType::While:
			AstWhileNodeDestructor(node->Node.While);
			break;
		case AstNodeType::FunctCall:
			AstFunctCallNodeDestructor(node->Node.FunctCall);
			break;
		default:
			break;
	}
	memset(node, 0, sizeof(InstructionNode));
}

void AstInstrBlockDestructor(ExtArray* const instrBlock)
{
	assert(instrBlock);
	
	const size_t nodesCount = instrBlock->Size;
	for (size_t st = 0; st < nodesCount; st++)
		AstInstrNodeDestructor((InstructionNode*)ExtArrayGetElemAt(instrBlock, st));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///