#ifndef LEXER_H
#define LEXER_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#include "../../Modules/ErrorsHandling.h"
#include "../../Modules/Text/Text.h"
#include "../../Modules/ExtArray/ExtArray.h"
#include "../../LanguageGrammar/LanguageGrammar.h"
#include "_identifier.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

struct Token
{
	LngTokenType Type;

	union
	{
		// Числовое значение лексемы. 
		int                Int;
		KeywordType       Keyword;
		OperatorType      Operator;
		int                Identifier;
		SpecialSymbolType SpecialSymbol;
		double             Number;
	} Value;
};

struct Lexer
{
	ExtArray Commands;

	Text Text;

	IdentifierTable IdentifierTable;
};

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus LexerConstructor(Lexer* lexer);

ProgramStatus LexerDestructor(Lexer* lexer);

ProgramStatus LexerGetTokens(Lexer* lexer);

ProgramStatus LexerReadFile(Lexer* lexer, const char* fileName);

ProgramStatus LexerLogDump(Lexer* lexer);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !LEXER_H