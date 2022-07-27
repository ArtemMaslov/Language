#ifndef LEXER_H
#define LEXER_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#include "../../LanguageGrammar/LanguageGrammar.h"

struct Identifier
{
	char*  Name;
	size_t NameLength;
};

struct Token
{
	TokenTypes Type;

	union
	{
		KeywordTypes       Keyword;
		OperatorTypes      Operator;
		int                Identifier;
		SpecialSymbolTypes SpecialSymbol;
	};
};

struct Lexer
{

};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#endif // !LEXER_H