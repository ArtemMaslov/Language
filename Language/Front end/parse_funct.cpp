///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль синтаксического анализатора. Функции поддержки функциональной парадигмы языка.
// 
// Файл с исходным кодом.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 14:26 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef PARSE_FUNCT_H
#define PARSE_FUNCT_H

#include <assert.h>

#include "Parser.h"
#include "parser_private.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

FunctCallNode* ParseFunctionCall(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Identifier)
	{
		return nullptr;
	}

	if (ELEM(parser->CurrentToken + 1)->Type != LngTokenType::SpecialSymbol ||
		ELEM(parser->CurrentToken + 1)->Value.SpecialSymbol != SpecialSymbolType::OpeningParenthesis)
	{
		return nullptr;
	}

	const int functNameId = ELEM(parser->CurrentToken)->Value.Identifier;
	FunctCallNode* node = AstCreateFunctCallNode(ast, functNameId);

	if (!node)
	{
		assert(!"Error");
		return nullptr;
	}

	parser->CurrentToken += 2;

	if (ParseFunctionCallParams(parser, &node->Params) != ProgramStatus::Ok)
	{
		AstFunctCallNodeDestructor(node);
		assert(!"Error");
		return nullptr;
	}

	return node;
}

ProgramStatus ParseFunctionCallParams(Parser* const parser, ExtArray* const outParams)
{
	assert(parser);
	assert(outParams);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	while (ELEM(parser->CurrentToken)->Type != LngTokenType::SpecialSymbol ||
		   ELEM(parser->CurrentToken)->Value.SpecialSymbol != SpecialSymbolType::ClosingParenthesis)
	{
		ExpressionNode* expr = ParseExpression(parser);
		if (!expr)
		{
			assert(!"Error");
			return ProgramStatus::Fault;
		}

		FunctParamNode* param = AstCreateFunctParamNode(ast, expr);
		if (!param)
		{
			assert(!"Error");
			return ProgramStatus::Fault;
		}
		ExtArrayAddElem(outParams, param);

		if (ELEM(parser->CurrentToken)->Type == LngTokenType::SpecialSymbol &&
			ELEM(parser->CurrentToken)->Value.SpecialSymbol == SpecialSymbolType::Comma)
		{
			parser->CurrentToken++;
		}
		else if (ELEM(parser->CurrentToken)->Type == LngTokenType::SpecialSymbol &&
				 ELEM(parser->CurrentToken)->Value.SpecialSymbol == SpecialSymbolType::ClosingParenthesis)
		{
			break;
		}
		else
		{
			assert(!"Error");
			return ProgramStatus::Fault;
		}
	}

	// Обрабатываем закрывающуюся круглую скобку ")".
	parser->CurrentToken++;

	return ProgramStatus::Ok;
}

ProgramStatus ParseFunctionDefParams(Parser* const parser, ExtArray* const outParams)
{
	assert(parser);
	assert(outParams);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	while (ELEM(parser->CurrentToken)->Type != LngTokenType::SpecialSymbol ||
		   ELEM(parser->CurrentToken)->Value.SpecialSymbol != SpecialSymbolType::ClosingParenthesis)
	{
		if (ELEM(parser->CurrentToken)->Type != LngTokenType::Identifier)
		{
			assert(!"Error");
			return ProgramStatus::Fault;
		}

		int nameId = ELEM(parser->CurrentToken)->Value.Identifier;
		FunctParamNode* param = AstCreateFunctParamNode(ast, nameId);
		if (!param)
		{
			assert(!"Error");
			return ProgramStatus::Fault;
		}
		ExtArrayAddElem(outParams, param);

		parser->CurrentToken++;

		if (ELEM(parser->CurrentToken)->Type == LngTokenType::SpecialSymbol &&
			ELEM(parser->CurrentToken)->Value.SpecialSymbol == SpecialSymbolType::Comma)
		{
			parser->CurrentToken++;
		}
		else if (ELEM(parser->CurrentToken)->Type == LngTokenType::SpecialSymbol &&
				 ELEM(parser->CurrentToken)->Value.SpecialSymbol == SpecialSymbolType::ClosingParenthesis)
		{
			break;
		}
		else
		{
			assert(!"Error");
			return ProgramStatus::Fault;
		}
	}

	// Обрабатываем закрывающуюся круглую скобку ")".
	parser->CurrentToken++;

	return ProgramStatus::Ok;
}

FunctDefNode* ParseFunctionDef(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Identifier)
	{
		return nullptr;
	}

	if (ELEM(parser->CurrentToken + 1)->Type != LngTokenType::SpecialSymbol ||
		ELEM(parser->CurrentToken + 1)->Value.SpecialSymbol != SpecialSymbolType::OpeningParenthesis)
	{
		return nullptr;
	}

	// "Идентификатор" + "(" => объявление функции.

	const int functNameId = ELEM(parser->CurrentToken)->Value.Identifier;
	FunctDefNode* node = AstCreateFunctDefNode(ast, functNameId);
	if (!node)
	{
		assert(!"Error");
		return nullptr;
	}

	parser->CurrentToken += 2;

	if (ParseFunctionDefParams(parser, &node->Params) != ProgramStatus::Ok)
	{
		AstFunctDefNodeDestructor(node);
		assert(!"Error");
		return nullptr;
	}

	if (ParseProgramBlock(parser, &node->Body) != ProgramStatus::Ok)
	{
		AstFunctDefNodeDestructor(node);
		assert(!"Error");
		return nullptr;
	}

	return node;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !PARSE_FUNCT_H