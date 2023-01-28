#ifndef PARSER_H
#define PARSER_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include "../Modules/ErrorsHandling.h"
#include "Lexer/Lexer.h"
#include "../AST/AST.h"

struct Parser
{
	Lexer Lexer;

	Token* Commands;
	size_t TokensCount;
	size_t CurrentToken;

	AST*   outAst;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus ParserConstructor(Parser* parser);

ProgramStatus ParserDestructor(Parser* parser);

ProgramStatus ParserParseFile(Parser* parser, AST* outAst, const char* fileName);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !PARSER_H