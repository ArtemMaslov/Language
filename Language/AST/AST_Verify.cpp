#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "AST.h"
#include "ast_verify_patterns.h"

#include "../LanguageGrammar/LanguageGrammar.h"
#include "../Modules/FilesIO/FilesIO.h"
#include "../Modules/Logs/Logs.h"
#include "../Front end/Lexer/Identifier.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define AST_DUMP_FOLDER LOGS_FOLDER "ast" PATH_SEPARATOR
#define AST_DUMP_FILE_NAME "ast_graphic_dump%zd"

struct AstDump
{
	FILE *File;
	size_t NodeIndex;
	const IdentifierTable *Identifiers;
	const static size_t BufferSize = 1024;
	char Buffer[BufferSize];
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static void DumpAstNode(AstDump *const dump, const AST *const ast);

static size_t DumpFunctCallNode(AstDump *const dump, const FunctCallNode *const node);

static size_t DumpFunctDefNode(AstDump *const dump, const FunctDefNode *const node);

static size_t DumpFunctParamNode(AstDump *const dump, const FunctParamNode *const node);

static size_t DumpBinOperNode(AstDump *const dump, const BinaryOperatorNode *const node);

static size_t DumpUnOperNode(AstDump *const dump, const UnaryOperatorNode *const node);

static size_t DumpExprNode(AstDump *const dump, const ExpressionNode *const node);

static size_t DumpVariableNode(AstDump *const dump, const VariableNode *const node);

static size_t DumpIfNode(AstDump *const dump, const IfNode *const node);

static size_t DumpWhileNode(AstDump *const dump, const WhileNode *const node);

static size_t DumpInstrNode(AstDump *const dump, const InstructionNode *const node);

static size_t DumpConstrNode(AstDump *const dump, const ConstructionNode *const node);

static size_t DumpInputNode(AstDump *const dump, const InputNode *const node);

static size_t DumpOutputNode(AstDump *const dump, const OutputNode *const node);

static size_t DumpReturnNode(AstDump *const dump, const ReturnNode *const node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static inline void DumpEdge(AstDump *const dump, const size_t parentId, const char *const parentName,
							const size_t childId, const char *const childName);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus AstGraphicDump(const AST *const ast)
{
	assert(ast);

	static size_t CallCount = 0;
	CallCount++;

	if (DirectoryExist(AST_DUMP_FOLDER) == FilesIoError::NotExist &&
		DirectoryCreate(AST_DUMP_FOLDER) != FilesIoError::NoErrors)
	{
		return ProgramStatus::Fault;
	}

	char path[256] = "";
	sprintf(path, AST_DUMP_FOLDER AST_DUMP_FILE_NAME ".gv", CallCount);

	AstDump dump = {};
	dump.NodeIndex = 1;
	dump.Identifiers = ast->Identifiers;
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

	char cmd[1024] = "";
	sprintf(cmd, "dot \"%s\" -Tpng > \"%s\"", path, imagePath);
	int status = system(cmd);

	if (status != 0)
		return ProgramStatus::Fault;

	return ProgramStatus::Ok;
}

#undef AST_GRAPHIC_DUMP_FOLDER

static void DumpAstNode(AstDump *const dump, const AST *const ast)
{
	assert(dump);
	assert(ast);

	const ExtArray *constrNodes = &ast->ConstrNodes;
	const size_t constrNodesCount = constrNodes->Size;

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpAstFormat,
			 nodeId);
	fputs(dump->Buffer, dump->File);

	size_t parentNodeId = nodeId;
	const char *parentNodeName = DumpAstNodeName;

	for (size_t st = 0; st < constrNodesCount; st++)
	{
		ConstructionNode *node = (ConstructionNode *)ExtArrayGetElemAt(constrNodes, st);

		size_t childNodeId = DumpConstrNode(dump, node);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpConstrNodeName);

		parentNodeId = childNodeId;
		parentNodeName = DumpConstrNodeName;
	}
}

static size_t DumpFunctCallNode(AstDump *const dump, const FunctCallNode *const node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpFunctCallFormat,
			 nodeId,
			 IdentifierGetById(dump->Identifiers, node->NameId)->Name);
	fputs(dump->Buffer, dump->File);

	size_t parentNodeId = nodeId;
	const char *parentNodeName = DumpFunctCallNodeName;
	const size_t paramsCount = node->Params.Size;

	for (size_t st = 0; st < paramsCount; st++)
	{
		FunctParamNode *functParamNode = (FunctParamNode *)ExtArrayGetElemAt(&node->Params, st);

		size_t childNodeId = DumpFunctParamNode(dump, functParamNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpFunctParamNodeName);

		parentNodeId = childNodeId;
		parentNodeName = DumpFunctParamNodeName;
	}

	return nodeId;
}

static size_t DumpFunctDefNode(AstDump *const dump, const FunctDefNode *const node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpFunctDefFormat,
			 nodeId,
			 IdentifierGetById(dump->Identifiers, node->NameId)->Name);
	fputs(dump->Buffer, dump->File);

	size_t parentNodeId = nodeId;
	const char *parentNodeName = DumpFunctDefNodeName;
	const size_t paramsCount = node->Params.Size;

	for (size_t st = 0; st < paramsCount; st++)
	{
		FunctParamNode *functParamNode = (FunctParamNode *)ExtArrayGetElemAt(&node->Params, st);

		size_t childNodeId = DumpFunctParamNode(dump, functParamNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpFunctParamNodeName);

		parentNodeId = childNodeId;
		parentNodeName = DumpFunctParamNodeName;
	}

	parentNodeId = nodeId;
	parentNodeName = DumpFunctDefNodeName;
	const size_t bodySize = node->Body.Size;

	for (size_t st = 0; st < bodySize; st++)
	{
		InstructionNode *instrNode = (InstructionNode *)ExtArrayGetElemAt(&node->Body, st);

		size_t childNodeId = DumpInstrNode(dump, instrNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpInstrNodeName);

		parentNodeId = childNodeId;
		parentNodeName = DumpInstrNodeName;
	}

	return nodeId;
}

static size_t DumpFunctParamNode(AstDump *const dump, const FunctParamNode *const node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	const char *paramName = IdentifierGetById(dump->Identifiers, node->NameId)->Name;

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

static size_t DumpBinOperNode(AstDump *const dump, const BinaryOperatorNode *const node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpBinOperFormat,
			 nodeId,
			 GrammarGetGraphVizName(node->Operator));
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = 0;

	childNodeId = DumpExprNode(dump, node->LeftOperand);

	DumpEdge(dump, nodeId, DumpBinOperNodeName, childNodeId, DumpExprNodeName);

	childNodeId = DumpExprNode(dump, node->RightOperand);

	DumpEdge(dump, nodeId, DumpBinOperNodeName, childNodeId, DumpExprNodeName);

	return nodeId;
}

static size_t DumpUnOperNode(AstDump *const dump, const UnaryOperatorNode *const node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpUnOperFormat,
			 nodeId,
			 GrammarGetToken(node->Operator)->Name);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = DumpExprNode(dump, node->Operand);

	DumpEdge(dump, nodeId, DumpUnOperNodeName, childNodeId, DumpExprNodeName);

	return nodeId;
}

static size_t DumpExprNode(AstDump *const dump, const ExpressionNode *const node)
{
	assert(dump);
	assert(node);

	double number = 0;

	size_t childNodeId = 0;
	const char *childNodeName = nullptr;

	const size_t nodeId = dump->NodeIndex++;

	switch (node->Type)
	{
	case AstNodeType::BinaryOperator:
		childNodeId = DumpBinOperNode(dump, node->Node.BinaryOperator);
		childNodeName = DumpBinOperNodeName;
		break;

	case AstNodeType::UnaryOperator:
		childNodeId = DumpUnOperNode(dump, node->Node.UnaryOperator);
		childNodeName = DumpUnOperNodeName;
		break;

	case AstNodeType::Number:
		number = node->Node.Number;
		break;

	case AstNodeType::FunctCall:
		childNodeId = DumpFunctCallNode(dump, node->Node.FunctCall);
		childNodeName = DumpFunctCallNodeName;
		break;

	case AstNodeType::Variable:
		childNodeId = DumpVariableNode(dump, node->Node.Variable);
		childNodeName = DumpVariableNodeName;
		break;

	default:
		assert(!"Unknown type");
		break;
	}

	if (node->Type != AstNodeType::Number)
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

static size_t DumpVariableNode(AstDump *const dump, const VariableNode *const node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpVariableFormat,
			 nodeId,
			 IdentifierGetById(dump->Identifiers, node->NameId)->Name);
	fputs(dump->Buffer, dump->File);

	if (node->InitValue)
	{
		size_t childNodeId = DumpExprNode(dump, node->InitValue);

		DumpEdge(dump, nodeId, DumpVariableNodeName, childNodeId, DumpExprNodeName);
	}

	return nodeId;
}

static size_t DumpIfNode(AstDump *const dump, const IfNode *const node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpIfFormat,
			 nodeId);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = DumpExprNode(dump, node->Condition);

	DumpEdge(dump, nodeId, DumpIfNodeName, childNodeId, DumpExprNodeName);

	size_t parentNodeId = nodeId;
	const char *parentNodeName = DumpIfNodeName;
	const size_t trueBlockSize = node->TrueBlock.Size;

	for (size_t st = 0; st < trueBlockSize; st++)
	{
		InstructionNode *instrNode = (InstructionNode *)ExtArrayGetElemAt(&node->TrueBlock, st);

		childNodeId = DumpInstrNode(dump, instrNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpInstrNodeName);

		parentNodeId = childNodeId;
		parentNodeName = DumpInstrNodeName;
	}

	parentNodeId = nodeId;
	parentNodeName = DumpIfNodeName;
	const size_t falseBlockSize = node->FalseBlock.Size;

	for (size_t st = 0; st < falseBlockSize; st++)
	{
		InstructionNode *instrNode = (InstructionNode *)ExtArrayGetElemAt(&node->FalseBlock, st);

		childNodeId = DumpInstrNode(dump, instrNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpInstrNodeName);

		parentNodeId = childNodeId;
		parentNodeName = DumpInstrNodeName;
	}

	return nodeId;
}

static size_t DumpWhileNode(AstDump *const dump, const WhileNode *const node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	snprintf(dump->Buffer, dump->BufferSize, DumpWhileFormat,
			 nodeId);
	fputs(dump->Buffer, dump->File);

	size_t childNodeId = DumpExprNode(dump, node->Condition);

	DumpEdge(dump, nodeId, DumpWhileNodeName, childNodeId, DumpExprNodeName);

	size_t parentNodeId = nodeId;
	const char *parentNodeName = DumpWhileNodeName;
	const size_t bodySize = node->Body.Size;

	for (size_t st = 0; st < bodySize; st++)
	{
		InstructionNode *instrNode = (InstructionNode *)ExtArrayGetElemAt(&node->Body, st);

		childNodeId = DumpInstrNode(dump, instrNode);

		DumpEdge(dump, parentNodeId, parentNodeName, childNodeId, DumpInstrNodeName);

		parentNodeId = childNodeId;
		parentNodeName = DumpInstrNodeName;
	}

	return nodeId;
}

static size_t DumpInstrNode(AstDump *const dump, const InstructionNode *const node)
{
	assert(dump);
	assert(node);

	size_t childNodeId = 0;
	const char *childNodeName = nullptr;

	switch (node->Type)
	{
	case AstNodeType::Variable:
		childNodeId = DumpVariableNode(dump, node->Node.Variable);
		childNodeName = DumpVariableNodeName;
		break;

	case AstNodeType::FunctCall:
		childNodeId = DumpFunctCallNode(dump, node->Node.FunctCall);
		childNodeName = DumpFunctCallNodeName;
		break;

	case AstNodeType::If:
		childNodeId = DumpIfNode(dump, node->Node.If);
		childNodeName = DumpIfNodeName;
		break;

	case AstNodeType::While:
		childNodeId = DumpWhileNode(dump, node->Node.While);
		childNodeName = DumpWhileNodeName;
		break;

	case AstNodeType::Input:
		childNodeId = DumpInputNode(dump, node->Node.Input);
		childNodeName = DumpInputNodeName;
		break;

	case AstNodeType::Output:
		childNodeId = DumpOutputNode(dump, node->Node.Output);
		childNodeName = DumpOutputNodeName;
		break;

	case AstNodeType::Return:
		childNodeId = DumpReturnNode(dump, node->Node.Return);
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

	if (node->Type != AstNodeType::Number)
		DumpEdge(dump, nodeId, DumpInstrNodeName, childNodeId, childNodeName);

	return nodeId;
}

static size_t DumpConstrNode(AstDump *const dump, const ConstructionNode *const node)
{
	assert(dump);
	assert(node);

	const size_t nodeId = dump->NodeIndex++;

	size_t childNodeId = 0;
	const char *childNodeName = nullptr;

	switch (node->Type)
	{
	case AstNodeType::GlobVar:
		childNodeId = DumpVariableNode(dump, node->Node.GlobVar);
		childNodeName = DumpVariableNodeName;
		break;

	case AstNodeType::FunctDef:
		childNodeId = DumpFunctDefNode(dump, node->Node.FunctDef);
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

static size_t DumpInputNode(AstDump *const dump, const InputNode *const node)
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

static size_t DumpOutputNode(AstDump *const dump, const OutputNode *const node)
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

static size_t DumpReturnNode(AstDump *const dump, const ReturnNode *const node)
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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static inline void DumpEdge(AstDump *const dump, const size_t parentId, const char *const parentName,
							const size_t childId, const char *const childName)
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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///