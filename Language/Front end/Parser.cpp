#include <assert.h>
#include <stdlib.h>

#include "Parser.h"
#include "Lexer/Lexer.h"
#include "../AST/AST.h"
#include "../Modules/ErrorsHandling.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define CHECK_STATUS \
	assert(status == ProgramStatus::Ok)
//if (status != ProgramStatus::Ok) \
//		return status

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ExpressionNode* ParseExpression(Parser* parser);

static ExpressionNode* ParseOr(Parser* parser);

static ExpressionNode* ParseAnd(Parser* parser);

static ExpressionNode* ParseNot(Parser* parser);

static ExpressionNode* ParseEqual(Parser* parser);

static ExpressionNode* ParseCompare(Parser* parser);

static ExpressionNode* ParseAddSub(Parser* parser);

static ExpressionNode* ParseMulDiv(Parser* parser);

static ExpressionNode* ParseUnaryAddSub(Parser* parser);

static ExpressionNode* Parse_Num_Var_Funct(Parser* parser);

static ExpressionNode* ParseParentheses(Parser* parser);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ExpressionNode* ParseCondition(Parser* parser);

static FunctCallNode* ParseFunctionCall(Parser* parser);

static ProgramStatus ParseFunctionCallParams(Parser* parser, ExtArray* outParams);

static ProgramStatus ParseFunctionDefParams(Parser* parser, ExtArray* outParams);

static FunctDefNode* ParseFunctionDef(Parser* parser);

static VariableNode* ParseVariable(Parser* parser);

static InputNode* ParseInput(Parser* parser);

static OutputNode* ParseOutput(Parser* parser);

static ReturnNode* ParseReturn(Parser* parser);

static WhileNode* ParseWhile(Parser* parser);

static IfNode* ParseIf(Parser* parser);

static ProgramStatus ParseProgramBlock(Parser* parser, ExtArray* instrBlock);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus ParserConstructor(Parser* parser)
{
	assert(parser);

	ProgramStatus status = ProgramStatus::Ok;

	status = LexerConstructor(&parser->Lexer);
	CHECK_STATUS;

	return status;
}

ProgramStatus ParserDestructor(Parser* parser)
{
	assert(parser);

	ProgramStatus status = ProgramStatus::Ok;

	status = LexerDestructor(&parser->Lexer);
	CHECK_STATUS;

	return status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus ParserParseFile(Parser* parser, AST* outAst, const char* fileName)
{
	assert(parser);
	assert(outAst);
	assert(fileName);
	
	ProgramStatus status = ProgramStatus::Ok;

	status = LexerReadFile(&parser->Lexer, fileName);
	CHECK_STATUS;

	status = LexerGetTokens(&parser->Lexer);
	CHECK_STATUS;
	
	outAst->IdentififerTable = parser->Lexer.IdentifierTable;

	parser->outAst = outAst;
	parser->Commands = (Token*)parser->Lexer.Commands.Array;
	parser->CurrentToken = 0;
	parser->TokensCount  = parser->Lexer.Commands.Size;

	while (parser->CurrentToken < parser->TokensCount)
	{
		FunctDefNode* functNode = ParseFunctionDef(parser);

		if (functNode)
		{
			ConstructionNode* node = AstCreateConstrNode(parser->outAst, functNode);

			ExtArrayAddElem(&parser->outAst->ConstrNodes, node);
			continue;
		}

		assert(!"Error");
		return ProgramStatus::Fault;
	}

	return ProgramStatus::Ok;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#include "_parse_expr.h"
#include "_parse_funct.h"
#include "_parse_keyword.h"
#include "_parse_aux.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 