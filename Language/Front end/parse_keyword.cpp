///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль синтаксического анализатора. Функции обработки ключевых слов.
// 
// Файл с исходным кодом.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 14:26 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef PARSE_KEYWORD_H
#define PARSE_KEYWORD_H

#include <assert.h>

#include "Parser.h"
#include "parser_private.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

IfNode* ParseIf(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Keyword ||
		ELEM(parser->CurrentToken)->Value.Keyword != KeywordType::If)
	{
		return nullptr;
	}

	parser->CurrentToken++;

	ExpressionNode* conditionNode = ParseCondition(parser);
	if (!conditionNode)
	{
		assert(!"Error");
		return nullptr;
	}

	IfNode* ifNode = AstCreateIfNode(ast, conditionNode);
	if (!ifNode)
	{
		assert(!"Error");
		return nullptr;
	}

	if (ParseProgramBlock(parser, &ifNode->TrueBlock) != ProgramStatus::Ok)
	{
		AstIfNodeDestructor(ifNode);
		assert(!"Error");
		return nullptr;
	}

	if (ELEM(parser->CurrentToken)->Type == LngTokenType::Keyword &&
		ELEM(parser->CurrentToken)->Value.Keyword == KeywordType::Else)
	{
		parser->CurrentToken++;

		if (ParseProgramBlock(parser, &ifNode->FalseBlock) != ProgramStatus::Ok)
		{
			AstIfNodeDestructor(ifNode);
			assert(!"Error");
			return nullptr;
		}
	}

	return ifNode;
}

InputNode* ParseInput(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Keyword ||
		ELEM(parser->CurrentToken)->Value.Keyword != KeywordType::Input)
	{
		return nullptr;
	}

	parser->CurrentToken += 1;

	//VariableNode* varNode = ParseVariable(parser);

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Identifier)
	{
		assert(!"Error");
		return nullptr;
	}

	int nameId = ELEM(parser->CurrentToken)->Value.Identifier;
	VariableNode* varNode = AstCreateVariableNode(ast, nameId, nullptr);
	if (!varNode)
	{
		assert(!"Error");
		return nullptr;
	}

	parser->CurrentToken++;

	InputNode* inputNode = AstCreateInputNode(ast, varNode);
	if (!inputNode)
	{
		assert(!"Error");
		return nullptr;
	}

	return inputNode;
}

OutputNode* ParseOutput(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Keyword ||
		ELEM(parser->CurrentToken)->Value.Keyword != KeywordType::Output)
	{
		return nullptr;
	}

	parser->CurrentToken += 1;

	ExpressionNode* exprNode = ParseExpression(parser);
	if (!exprNode)
	{
		assert(!"Error");
		return nullptr;
	}

	OutputNode* outputNode = AstCreateOutputNode(ast, exprNode);

	return outputNode;
}

ReturnNode* ParseReturn(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Keyword ||
		ELEM(parser->CurrentToken)->Value.Keyword != KeywordType::Return)
	{
		return nullptr;
	}

	parser->CurrentToken++;

	ExpressionNode* exprNode = ParseExpression(parser);

	if (!exprNode)
	{
		assert(!"Error");
		return nullptr;
	}

	ReturnNode* retNode = AstCreateReturnNode(ast, exprNode);

	if (!retNode)
	{
		assert(!"Error");
		return nullptr;
	}

	return retNode;
}

WhileNode* ParseWhile(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Keyword ||
		ELEM(parser->CurrentToken)->Value.Keyword != KeywordType::While)
	{
		return nullptr;
	}

	parser->CurrentToken++;

	ExpressionNode* conditionNode = ParseCondition(parser);
	if (!conditionNode)
	{
		assert(!"Error");
		return nullptr;
	}

	WhileNode* whileNode = AstCreateWhileNode(ast, conditionNode);
	if (!whileNode)
	{
		assert(!"Error");
		return nullptr;
	}

	if (ParseProgramBlock(parser, &whileNode->Body) != ProgramStatus::Ok)
	{
		AstWhileNodeDestructor(whileNode);
		assert(!"Error");
		return nullptr;
	}

	return whileNode;
}

/// Condition - Это ExpressionNode, обязательно окруженная ().
ExpressionNode* ParseCondition(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::SpecialSymbol ||
		ELEM(parser->CurrentToken)->Value.SpecialSymbol != SpecialSymbolType::OpeningParenthesis)
	{
		return nullptr;
	}

	parser->CurrentToken++;

	ExpressionNode* conditionNode = ParseExpression(parser);
	if (!conditionNode)
	{
		assert(!"Error");
		return nullptr;
	}

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::SpecialSymbol ||
		ELEM(parser->CurrentToken)->Value.SpecialSymbol != SpecialSymbolType::ClosingParenthesis)
	{
		assert(!"Error");
		return nullptr;
	}

	parser->CurrentToken++;

	return conditionNode;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !PARSE_KEYWORD_H