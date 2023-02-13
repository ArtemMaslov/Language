///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Файл грамматики языка. 
// 
// Определение грамматических массивов и функций для работы с ними.
//
// Версия: 1.0.1.1
// Дата последнего изменения: 11:21 03.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>

#include "LanguageGrammar.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define GRAMMAR(type, grammar, oper, name, ...)					\
	{															\
		.Type           = LngTokenType::type,					\
		.Grammar        = GrammarType::grammar,					\
		.Value          = {.Operator = OperatorType::oper},		\
		.Name           = name,									\
		.NameSize       = sizeof(name) / sizeof(char) - 1		\
	},

GrammarToken Operators[] =
{
#include "operators.inc"
};

#undef GRAMMAR

#define GRAMMAR(type, grammar, oper, name, nameGV)				\
	nameGV,

/// Название оператора для GraphViz.
static const char* const OperatorsGV[] =
{
#include "operators.inc"
};

#undef GRAMMAR

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define GRAMMAR(type, grammar, specSym, name)									\
	{																			\
		.Type                = LngTokenType::type,								\
		.Grammar             = GrammarType::grammar,							\
		.Value               = {.SpecialSymbol = SpecialSymbolType::specSym},	\
		.Name                = name,											\
		.NameSize            = sizeof(name) / sizeof(char) - 1					\
	},

GrammarToken SpecialSymbols[] =
{
#include "special_symbols.inc"
};

#undef GRAMMAR

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define GRAMMAR(type, grammar, keyword, name)					\
	{															\
		.Type           = LngTokenType::type,					\
		.Grammar        = GrammarType::grammar,					\
		.Value          = {.Keyword = KeywordType::keyword},	\
		.Name           = name,									\
		.NameSize       = sizeof(name) / sizeof(char)- 1		\
	},

GrammarToken Keywords[] =
{
#include "keywords.inc"
};

#undef GRAMMAR

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

const size_t KeywordsSize       = sizeof(Keywords)       / sizeof(GrammarToken);
const size_t SpecialSymbolsSize = sizeof(SpecialSymbols) / sizeof(GrammarToken);
const size_t OperatorsSize      = sizeof(Operators)      / sizeof(GrammarToken);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

const GrammarToken* GrammarGetToken(const KeywordType keyword)
{
	assert((size_t)keyword < KeywordsSize);

	return Keywords + (size_t)keyword;
}

const GrammarToken* GrammarGetToken(const OperatorType oper)
{
	assert((size_t)oper < OperatorsSize);

	return Operators + (size_t)oper;
}

const GrammarToken* GrammarGetToken(const SpecialSymbolType specSym)
{
	assert((size_t)specSym < SpecialSymbolsSize);

	return SpecialSymbols + (size_t)specSym;
}

const char* const GrammarGetGraphVizName(const OperatorType oper)
{
	assert((size_t)oper < OperatorsSize);

	return OperatorsGV[(size_t)oper];
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///