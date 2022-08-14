#include <assert.h>

#include "LanguageGrammar.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define GRAMMAR(type, grammar, oper, name)					\
	{														\
		.Type           = type,								\
		.Grammar        = grammar,							\
		.Value          = {.Operator = oper},				\
		.Name           = name,								\
		.NameSize       = sizeof(name) / sizeof(char) - 1	\
	},

GrammarToken Operators[] =
{
#include "Universal/Operators.inc"
};

#undef GRAMMAR

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define GRAMMAR(type, grammar, specSym, name)					\
	{															\
		.Type                = type,							\
		.Grammar             = grammar,							\
		.Value               = {.SpecialSymbol = specSym},		\
		.Name                = name,							\
		.NameSize            = sizeof(name) / sizeof(char) - 1	\
	},

GrammarToken SpecialSymbols[] =
{
#include "Universal/SpecialSymbols.inc"
};

#undef GRAMMAR

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define GRAMMAR(type, grammar, keyword, name)				\
	{														\
		.Type           = type,								\
		.Grammar        = grammar,							\
		.Value          = {.Keyword = keyword},				\
		.Name           = name,								\
		.NameSize       = sizeof(name) / sizeof(char)- 1	\
	},

GrammarToken Keywords[] =
{
#include "English/Keywords.inc"
};

#undef GRAMMAR

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const size_t KeywordsSize       = sizeof(Keywords)       / sizeof(GrammarToken);
const size_t SpecialSymbolsSize = sizeof(SpecialSymbols) / sizeof(GrammarToken);
const size_t OperatorsSize      = sizeof(Operators)      / sizeof(GrammarToken);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const GrammarToken* GrammarGetName(LngTokenType type, int id)
{
	switch (type)
	{
		case LngTokenType::Keyword:
			for (size_t st = 0; st < KeywordsSize; st++)
			{
				if (Keywords[st].Value.Int == id)
					return Keywords + st;
			}
			break;

		case LngTokenType::Operator:
			for (size_t st = 0; st < OperatorsSize; st++)
			{
				if (Operators[st].Value.Int == id)
					return Operators + st;
			}
			break;

		case LngTokenType::SpecialSymbol:
			for (size_t st = 0; st < SpecialSymbolsSize; st++)
			{
				if (SpecialSymbols[st].Value.Int == id)
					return SpecialSymbols + st;
			}
			break;

		//case LngTokenType::Number:
		//case LngTokenType::Identifier:
		default:
			break;
	}

	assert(!"ѕо данному типу лексемы нельз€ получить им€!");
	return nullptr;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 