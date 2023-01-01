#include <assert.h>
#include <stdio.h>
#include <Windows.h>

#include "AST.h"
#include "AST_Verify.h"
#include "_ast_verify_patterns.h"

#include "../LanguageGrammar/LanguageGrammar.h"
#include "../Modules/FilesIO/FilesIO.h"
#include "../Modules/Logs/Logs.h"
#include "../Front end/Lexer/_identifier.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define AST_DUMP_FOLDER    LOGS_FOLDER "ast\\"
#define AST_DUMP_FILE_NAME "ast_graphic_dump%zd"

struct AstDump
{
	FILE* File;
	size_t NodeIndex;
	const IdentifierTable* IdentifierTable;
	const static size_t BufferSize = 1024;
	char  Buffer[BufferSize];
};

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static void DumpAstNode(AstDump* dump, const AST* ast);

static size_t DumpFunctCallNode(AstDump* dump, const FunctCallNode* node);

static size_t DumpFunctDefNode(AstDump* dump, const FunctDefNode* node);

static size_t DumpFunctParamNode(AstDump* dump, const FunctParamNode* node);

static size_t DumpBinOperNode(AstDump* dump, const BinaryOperatorNode* node);

static size_t DumpUnOperNode(AstDump* dump, const UnaryOperatorNode* node);

static size_t DumpExprNode(AstDump* dump, const ExpressionNode* node);

static size_t DumpVariableNode(AstDump* dump, const VariableNode* node);

static size_t DumpIfNode(AstDump* dump, const IfNode* node);

static size_t DumpWhileNode(AstDump* dump, const WhileNode* node);

static size_t DumpInstrNode(AstDump* dump, const InstructionNode* node);

static size_t DumpConstrNode(AstDump* dump, const ConstructionNode* node);

static size_t DumpInputNode(AstDump* dump, const InputNode* node);

static size_t DumpOutputNode(AstDump* dump, const OutputNode* node);

static size_t DumpReturnNode(AstDump* dump, const ReturnNode* node);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static inline void DumpEdge(AstDump* dump, const size_t parentId, const char* parentName, const size_t childId, const char* childName);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus AstGraphicDump(const AST* ast)
{
	assert(ast);

	static size_t CallCount = 0;
	CallCount++;

	if (!DirectoryExist(AST_DUMP_FOLDER) &&
		!CreateDirectoryA(AST_DUMP_FOLDER, nullptr))
	{
		return ProgramStatus::Fault;
	}

	char path[256] = "";
	sprintf(path, AST_DUMP_FOLDER AST_DUMP_FILE_NAME ".gv", CallCount);

	AstDump dump = {};
	dump.NodeIndex = 1;
	dump.IdentifierTable = &ast->IdentififerTable;
	dump.File = fopen(path, "w");

	if (!dump.File)
	{
		return ProgramStatus::Fault;
	}

	fputs(DumpGraphHeader, dump.File);

	DumpAstNode(&dump, ast);

	fputc('}', dump.File);

	fclose(dump.File);

	char imagePath[256] = "";
	sprintf(imagePath, AST_DUMP_FOLDER AST_DUMP_FILE_NAME ".png", CallCount);

	char cmd[256] = "";
	sprintf(cmd, "dot \"%s\" -Tpng > \"%s\"", path, imagePath);
	int status = system(cmd);

	if (status != 0)
		return ProgramStatus::Fault;

	return ProgramStatus::Ok;
}

#undef AST_GRAPHIC_DUMP_FOLDER

static void DumpAstNode(AstDump* dump, const AST* ast)
{
	assert(dump);
	assert(ast);

	const ExtArray* constrNodes = &ast->ConstrNodes;
	const size_t constrNodesCount = constrNodes->Size;

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpAstFormat,
			 nodeId);
	fputs(dump->Buffer, dump->File);

	size_t      parentNodeId   = nodeId;
	const char* parentNodeName = DumpAstNodeName;

	for (size_t st = 0; st < constrNodesCount; st++)
	{
		ConstructionNode* node = (ConstructionNode*)ExtArrayGetElemAt(constrNodes, st);

		size_t childNodeId = DumpConstrNode(dump, node);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpConstrNodeName);

		parentNodeId   = childNodeId;
		parentNodeName = DumpConstrNodeName;
	}
}

static size_t DumpFunctCallNode(AstDump* dump, const FunctCallNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpFunctCallFormat,
			 nodeId,
			 IdentifierGetById(dump->IdentifierTable, node->NameId)->Name);
	fputs(dump->Buffer, dump->File);

	size_t       parentNodeId   = nodeId;
	const char*  parentNodeName = DumpFunctCallNodeName;
	const size_t paramsCount    = node->Params.Size;

	for (size_t st = 0; st < paramsCount; st++)
	{
		FunctParamNode* functParamNode = (FunctParamNode*)ExtArrayGetElemAt(&node->Params, st);

		size_t childNodeId = DumpFunctParamNode(dump, functParamNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpFunctParamNodeName);

		parentNodeId   = childNodeId;
		parentNodeName = DumpFunctParamNodeName;
	}

	return nodeId;
}

static size_t DumpFunctDefNode(AstDump* dump, const FunctDefNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpFunctDefFormat,
			 nodeId,
			 IdentifierGetById(dump->IdentifierTable, node->NameId)->Name);
	fputs(dump->Buffer, dump->File);

	size_t       parentNodeId   = nodeId;
	const char*  parentNodeName = DumpFunctDefNodeName;
	const size_t paramsCount    = node->Params.Size;

	for (size_t st = 0; st < paramsCount; st++)
	{
		FunctParamNode* functParamNode = (FunctParamNode*)ExtArrayGetElemAt(&node->Params, st);

		size_t childNodeId = DumpFunctParamNode(dump, functParamNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpFunctParamNodeName);

		parentNodeId   = childNodeId;
		parentNodeName = DumpFunctParamNodeName;
	}

	parentNodeId   = nodeId;
	parentNodeName = DumpFunctDefNodeName;
	const size_t bodySize = node->Body.Size;

	for (size_t st = 0; st < bodySize; st++)
	{
		InstructionNode* instrNode = (InstructionNode*)ExtArrayGetElemAt(&node->Body, st);

		size_t childNodeId = DumpInstrNode(dump, instrNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpInstrNodeName);

		parentNodeId   = childNodeId;
		parentNodeName = DumpInstrNodeName;
	}

	return nodeId;
}

static size_t DumpFunctParamNode(AstDump* dump, const FunctParamNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	const char* paramName = IdentifierGetById(dump->IdentifierTable, node->NameId)->Name;

	if (!paramName)
		paramName = "";

	snprintf(dump->Buffer, dump->BufferSize, DumpFunctParamFormat,
			 nodeId,
			 paramName);
	fputs(dump->Buffer, dump->File);

	if (node->Value)
	{
		size_t childNodeId = DumpExprNode(dump, node->Value);

		DumpEdge(dump, nodeId, DumpFunctParamNodeName, childNodeId, DumpExprNodeName);
	}

	return nodeId;
}

static size_t DumpBinOperNode(AstDump* dump, const BinaryOperatorNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpBinOperFormat,
			 nodeId,
			 GrammarGetName(LngTokenType::Operator, (int)node->Operator)->Name);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId  = 0;

	childNodeId = DumpExprNode(dump, node->LeftOperand);

	DumpEdge(dump, nodeId, DumpBinOperNodeName, childNodeId, DumpExprNodeName);

	childNodeId = DumpExprNode(dump, node->RightOperand);

	DumpEdge(dump, nodeId, DumpBinOperNodeName, childNodeId, DumpExprNodeName);

	return nodeId;
}

static size_t DumpUnOperNode(AstDump* dump, const UnaryOperatorNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpUnOperFormat,
			 nodeId,
			 GrammarGetName(LngTokenType::Operator, (int)node->Operator)->Name);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = DumpExprNode(dump, node->Operand);

	DumpEdge(dump, nodeId, DumpUnOperNodeName, childNodeId, DumpExprNodeName);

	return nodeId;
}

static size_t DumpExprNode(AstDump* dump, const ExpressionNode* node)
{
	assert(dump);
	assert(node);

	double number = 0;

	size_t      childNodeId   = 0;
	const char* childNodeName = nullptr;

	const size_t nodeId = dump->NodeIndex++;

	switch (node->Type)
	{
		case AstNodeTypes::BinaryOperator:
			childNodeId   = DumpBinOperNode(dump, node->Node.BinaryOperator);
			childNodeName = DumpBinOperNodeName;
			break;

		case AstNodeTypes::UnaryOperator:
			childNodeId   = DumpUnOperNode(dump, node->Node.UnaryOperator);
			childNodeName = DumpUnOperNodeName;
			break;

		case AstNodeTypes::Number:
			number = node->Node.Number;
			break;

		case AstNodeTypes::FunctCall:
			childNodeId   = DumpFunctCallNode(dump, node->Node.FunctCall);
			childNodeName = DumpFunctCallNodeName;
			break;

		case AstNodeTypes::Variable:
			childNodeId   = DumpVariableNode(dump, node->Node.Variable);
			childNodeName = DumpVariableNodeName;
			break;

		default:
			assert(!"Unknown type");
			break;
	}

	if (node->Type != AstNodeTypes::Number)
	{
		snprintf(dump->Buffer, dump->BufferSize, DumpExprFormat,
				 nodeId,
				 AstNodeGetName(node->Type));
	}
	else
	{
		snprintf(dump->Buffer, dump->BufferSize, DumpExprNumFormat,
				 nodeId,
				 AstNodeGetName(node->Type),
				 number);
	}
	fputs(dump->Buffer, dump->File);

	if (childNodeName)
	{
		DumpEdge(dump, nodeId, DumpExprNodeName, childNodeId, childNodeName);
	}

	return nodeId;
}

static size_t DumpVariableNode(AstDump* dump, const VariableNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpVariableFormat,
			 nodeId,
			 IdentifierGetById(dump->IdentifierTable, node->NameId)->Name);
	fputs(dump->Buffer, dump->File);

	if (node->InitValue)
	{
		size_t childNodeId = DumpExprNode(dump, node->InitValue);

		DumpEdge(dump, nodeId, DumpVariableNodeName, childNodeId, DumpExprNodeName);
	}

	return nodeId;
}

static size_t DumpIfNode(AstDump* dump, const IfNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpIfFormat,
			 nodeId);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = DumpExprNode(dump, node->Condition);

	DumpEdge(dump, nodeId, DumpIfNodeName, childNodeId, DumpExprNodeName);

	size_t       parentNodeId   = nodeId;
	const char*  parentNodeName = DumpIfNodeName;
	const size_t trueBlockSize  = node->TrueBlock.Size;

	for (size_t st = 0; st < trueBlockSize; st++)
	{
		InstructionNode* instrNode = (InstructionNode*)ExtArrayGetElemAt(&node->TrueBlock, st);

		size_t childNodeId = DumpInstrNode(dump, instrNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpInstrNodeName);

		parentNodeId   = childNodeId;
		parentNodeName = DumpInstrNodeName;
	}

	parentNodeId   = nodeId;
	parentNodeName = DumpIfNodeName;
	const size_t falseBlockSize = node->FalseBlock.Size;

	for (size_t st = 0; st < falseBlockSize; st++)
	{
		InstructionNode* instrNode = (InstructionNode*)ExtArrayGetElemAt(&node->FalseBlock, st);

		size_t childNodeId = DumpInstrNode(dump, instrNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpInstrNodeName);

		parentNodeId   = childNodeId;
		parentNodeName = DumpInstrNodeName;
	}

	return nodeId;
}

static size_t DumpWhileNode(AstDump* dump, const WhileNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpWhileFormat,
			 nodeId);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = DumpExprNode(dump, node->Condition);

	DumpEdge(dump, nodeId, DumpWhileNodeName, childNodeId, DumpExprNodeName);

	size_t       parentNodeId   = nodeId;
	const char*  parentNodeName = DumpWhileNodeName;
	const size_t bodySize       = node->Body.Size;

	for (size_t st = 0; st < bodySize; st++)
	{
		InstructionNode* instrNode = (InstructionNode*)ExtArrayGetElemAt(&node->Body, st);

		size_t childNodeId = DumpInstrNode(dump, instrNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpInstrNodeName);

		parentNodeId   = childNodeId;
		parentNodeName = DumpInstrNodeName;
	}

	return nodeId;
}

static size_t DumpInstrNode(AstDump* dump, const InstructionNode* node)
{
	assert(dump);
	assert(node);

	size_t      childNodeId   = 0;
	const char* childNodeName = nullptr;

	switch (node->Type)
	{
		case AstNodeTypes::Variable:
			childNodeId   = DumpVariableNode(dump, node->Node.Variable);
			childNodeName = DumpVariableNodeName;
			break;

		case AstNodeTypes::FunctCall:
			childNodeId   = DumpFunctCallNode(dump, node->Node.FunctCall);
			childNodeName = DumpFunctCallNodeName;
			break;

		case AstNodeTypes::If:
			childNodeId   = DumpIfNode(dump, node->Node.If);
			childNodeName = DumpIfNodeName;
			break;

		case AstNodeTypes::While:
			childNodeId   = DumpWhileNode(dump, node->Node.While);
			childNodeName = DumpWhileNodeName;
			break;

		case AstNodeTypes::Input:
			childNodeId   = DumpInputNode(dump, node->Node.Input);
			childNodeName = DumpInputNodeName;
			break;

		case AstNodeTypes::Output:
			childNodeId   = DumpOutputNode(dump, node->Node.Output);
			childNodeName = DumpOutputNodeName;
			break;

		case AstNodeTypes::Return:
			childNodeId   = DumpReturnNode(dump, node->Node.Return);
			childNodeName = DumpReturnNodeName;
			break;

		default:
			assert(!"Unknown type");
			break;
	}

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpInstrFormat,
			 nodeId,
			 AstNodeGetName(node->Type));
	fputs(dump->Buffer, dump->File);

	if (node->Type != AstNodeTypes::Number)
		DumpEdge(dump, nodeId, DumpInstrNodeName, childNodeId, childNodeName);

	return nodeId;
}

static size_t DumpConstrNode(AstDump* dump, const ConstructionNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	size_t      childNodeId   = 0;
	const char* childNodeName = nullptr;

	switch (node->Type)
	{
		case AstNodeTypes::GlobVar:
			childNodeId   = DumpVariableNode(dump, node->Node.GlobVar);
			childNodeName = DumpVariableNodeName;
			break;

		case AstNodeTypes::FunctDef:
			childNodeId   = DumpFunctDefNode(dump, node->Node.FunctCall);
			childNodeName = DumpFunctDefNodeName;
			break;

		default:
			assert(!"Unknown type");
			break;
	}

	snprintf(dump->Buffer, dump->BufferSize, DumpConstrFormat,
			 nodeId,
			 AstNodeGetName(node->Type));
	fputs(dump->Buffer, dump->File);

	DumpEdge(dump, nodeId, DumpConstrNodeName, childNodeId, childNodeName);

	return nodeId;
}

static size_t DumpInputNode(AstDump* dump, const InputNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpInputFormat,
			 nodeId);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = DumpVariableNode(dump, node->Input);

	DumpEdge(dump, nodeId, DumpInputNodeName, childNodeId, DumpVariableNodeName);

	return nodeId;
}

static size_t DumpOutputNode(AstDump* dump, const OutputNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpOutputFormat,
			 nodeId);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = DumpExprNode(dump, node->Output);

	DumpEdge(dump, nodeId, DumpOutputNodeName, childNodeId, DumpExprNodeName);

	return nodeId;
}

static size_t DumpReturnNode(AstDump* dump, const ReturnNode* node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpReturnFormat,
			 nodeId);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = DumpExprNode(dump, node->Value);

	DumpEdge(dump, nodeId, DumpReturnNodeName, childNodeId, DumpExprNodeName);

	return nodeId;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static inline void DumpEdge(AstDump* dump, const size_t parentId, const char* parentName, const size_t childId, const char* childName)
{
	assert(dump);
	assert(parentName);
	assert(childName);

	snprintf(dump->Buffer, dump->BufferSize, DumpEdgeFormat,
			 parentName,
			 parentId,
			 childName,
			 childId);
	fputs(dump->Buffer, dump->File);
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 