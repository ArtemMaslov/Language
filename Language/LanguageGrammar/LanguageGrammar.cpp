#include "LanguageGrammar.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#define GRAMMAR(type, grammar, oper, name) \
	{type, grammar, oper, name},

GrammarToken Operators[] =
{
#include "Universal/Operators.inc"
};

GrammarToken SpecialSymbols[] =
{
#include "Universal/SpecialSymbols.inc"
};

GrammarToken Keywords[] =
{
#include "English/Keywords.inc"
};

#undef GRAMMAR

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\