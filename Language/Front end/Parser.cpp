//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль синтаксического анализатора. Основные функции.
// 
// Файл с исходным кодом.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 13:08 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>

#include "../Modules/Logs/Logs.h"
#include "../Modules/ErrorsHandling.h"

#include "Lexer/Lexer.h"
#include "../AST/AST.h"

#include "Parser.h"
#include "parser_private.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ParserError ParserParseFile(Lexer* const lexer, AST* const outAst)
{
	assert(lexer);
	assert(outAst);

	Parser parser = 
	{
		.Tokens       = &lexer->Tokens,
		.CurrentToken = 0,
		.TokensCount  = lexer->Tokens.Size,
		.OutAst       = outAst
	};
	outAst->Identifiers = lexer->Identifiers;

	while (parser.CurrentToken < parser.TokensCount)
	{
		FunctDefNode* functNode = ParseFunctionDef(&parser);

		if (functNode)
		{
			ConstructionNode* node = AstCreateConstrNode(parser.OutAst, functNode);

			ExtArrayAddElem(&parser.OutAst->ConstrNodes, node);
			continue;
		}

		LOG_ERR("Не верная синтаксическая конструкция.");
		return ParserError::SyntaxError;
	}

	return ParserError::NoErrors;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///