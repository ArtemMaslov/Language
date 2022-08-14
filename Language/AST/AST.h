#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#include "../LanguageGrammar/LanguageGrammar.h"
#include "../Modules/ExtArray/ExtArray.h"
#include "../Modules/ExtHeap/ExtHeap.h"
#include "../Front end//Lexer/_identifier.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

struct FunctCallNode;
struct FunctParamNode;
struct FunctDefNode;
struct BinaryOperatorNode;
struct UnaryOperatorNode;
struct ExpressionNode;
struct VariableNode;
struct IfNode;
struct WhileNode;
struct InstructionNode;
struct ConstructionNode;
struct InputNode;
struct OutputNode;

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

enum class AstNodeTypes
{
	Variable	   = 0,
	GlobVar        = 0,
	Number         = 1,

	FunctDef       = 10,
	FunctCall      = 11,
	FunctParamNode = 12,
	Return         = 13,

	Expression     = 20,
	BinaryOperator = 21,
	UnaryOperator  = 22,

	If             = 30,

	While          = 40,

	Input          = 50,
	Output         = 51,

	Instruction    = 100,
	Construction   = 101
};

struct AST
{
	ExtArray ConstrNodes;

	ExtHeap  Nodes;

	IdentifierTable IdentififerTable;
};

const size_t AST_NODES_HEAP_DEFAULT_SIZE = 40960;

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#include "_expr_nodes.h"
#include "_funct_nodes.h"
#include "_keyword_nodes.h"
#include "_aux_nodes.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus AstConstructor(AST* ast);

ProgramStatus AstDestructor(AST* ast);

const char* AstNodeGetName(AstNodeTypes type);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !ABSTRACT_SYNTAX_TREE_H