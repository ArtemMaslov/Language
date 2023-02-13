///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль синтаксического анализатора. Функции работы с выражениями.
// 
// Файл с исходным кодом.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 14:26 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef PARSE_EXPR_H
#define PARSE_EXPR_H

#include <assert.h>

#include "Parser.h"
#include "parser_private.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ExpressionNode* ParseExpression(Parser* const parser)
{
	assert(parser);

	ExpressionNode* exprNode = ParseOr(parser);

	return exprNode;
}

ExpressionNode* ParseOr(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	ExpressionNode* leftOperand  = nullptr;
	ExpressionNode* rightOperand = nullptr;
	OperatorType oper = OperatorType::Null;

	do
	{
		rightOperand = ParseAnd(parser);

		if (leftOperand)
		{
			BinaryOperatorNode* binOperNode = AstCreateBinaryOperatorNode(ast, oper, leftOperand, rightOperand);

			if (!binOperNode)
			{
				assert(!"Error");
				return nullptr;
			}

			ExpressionNode* exprNode = AstCreateExpressionNode(ast, binOperNode);

			if (!exprNode)
			{
				assert(!"Error");
				return nullptr;
			}

			leftOperand = exprNode;
		}
		else
			leftOperand = rightOperand;

		if (ELEM(parser->CurrentToken)->Type == LngTokenType::Operator &&
			(ELEM(parser->CurrentToken)->Value.Operator == OperatorType::Or))
		{
			oper = ELEM(parser->CurrentToken)->Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

ExpressionNode* ParseAnd(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	ExpressionNode* leftOperand  = nullptr;
	ExpressionNode* rightOperand = nullptr;
	OperatorType oper = OperatorType::Null;

	do
	{
		rightOperand = ParseNot(parser);

		if (leftOperand)
		{
			BinaryOperatorNode* binOperNode = AstCreateBinaryOperatorNode(ast, oper, leftOperand, rightOperand);

			if (!binOperNode)
			{
				assert(!"Error");
				return nullptr;
			}

			ExpressionNode* exprNode = AstCreateExpressionNode(ast, binOperNode);

			if (!exprNode)
			{
				assert(!"Error");
				return nullptr;
			}

			leftOperand = exprNode;
		}
		else
			leftOperand = rightOperand;

		if (ELEM(parser->CurrentToken)->Type == LngTokenType::Operator &&
			(ELEM(parser->CurrentToken)->Value.Operator == OperatorType::And))
		{
			oper = ELEM(parser->CurrentToken)->Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

ExpressionNode* ParseNot(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Operator ||
		ELEM(parser->CurrentToken)->Value.Operator != OperatorType::Not)
	{
		return ParseEqual(parser);
	}

	parser->CurrentToken += 1;

	ExpressionNode* operandNode = ParseEqual(parser);

	if (!operandNode)
	{
		assert(!"Error");
		return nullptr;
	}

	UnaryOperatorNode* unOperNode = AstCreateUnaryOperatorNode(ast, OperatorType::Not, operandNode);

	if (!unOperNode)
	{
		assert(!"Error");
		return nullptr;
	}

	ExpressionNode* exprNode = AstCreateExpressionNode(ast, unOperNode);

	if (!exprNode)
	{
		assert(!"Error");
		return nullptr;
	}

	return exprNode;
}

/// ==, !=
ExpressionNode* ParseEqual(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	ExpressionNode* leftOperand  = nullptr;
	ExpressionNode* rightOperand = nullptr;
	OperatorType oper = OperatorType::Null;

	do
	{
		rightOperand = ParseCompare(parser);

		if (leftOperand)
		{
			BinaryOperatorNode* binOperNode = AstCreateBinaryOperatorNode(ast, oper, leftOperand, rightOperand);

			if (!binOperNode)
			{
				assert(!"Error");
				return nullptr;
			}

			ExpressionNode* exprNode = AstCreateExpressionNode(ast, binOperNode);

			if (!exprNode)
			{
				assert(!"Error");
				return nullptr;
			}

			leftOperand = exprNode;
		}
		else
			leftOperand = rightOperand;

		if (ELEM(parser->CurrentToken)->Type == LngTokenType::Operator &&
			(ELEM(parser->CurrentToken)->Value.Operator == OperatorType::Equal ||
			 ELEM(parser->CurrentToken)->Value.Operator == OperatorType::NotEqual))
		{
			oper = ELEM(parser->CurrentToken)->Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

/// >, <, >=, <=
ExpressionNode* ParseCompare(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	ExpressionNode* leftOperand  = nullptr;
	ExpressionNode* rightOperand = nullptr;
	OperatorType oper = OperatorType::Null;

	do
	{
		rightOperand = ParseAddSub(parser);

		if (leftOperand)
		{
			BinaryOperatorNode* binOperNode = AstCreateBinaryOperatorNode(ast, oper, leftOperand, rightOperand);

			if (!binOperNode)
			{
				assert(!"Error");
				return nullptr;
			}

			ExpressionNode* exprNode = AstCreateExpressionNode(ast, binOperNode);

			if (!exprNode)
			{
				assert(!"Error");
				return nullptr;
			}

			leftOperand = exprNode;
		}
		else
			leftOperand = rightOperand;

		if (ELEM(parser->CurrentToken)->Type == LngTokenType::Operator &&
			(ELEM(parser->CurrentToken)->Value.Operator == OperatorType::Greater ||
			 ELEM(parser->CurrentToken)->Value.Operator == OperatorType::GreaterEqual ||
			 ELEM(parser->CurrentToken)->Value.Operator == OperatorType::Less ||
			 ELEM(parser->CurrentToken)->Value.Operator == OperatorType::LessEqual))
		{
			oper = ELEM(parser->CurrentToken)->Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

ExpressionNode* ParseAddSub(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	ExpressionNode* leftOperand  = nullptr;
	ExpressionNode* rightOperand = nullptr;
	OperatorType oper = OperatorType::Null;

	do
	{
		rightOperand = ParseMulDiv(parser);

		if (leftOperand)
		{
			BinaryOperatorNode* binOperNode = AstCreateBinaryOperatorNode(ast, oper, leftOperand, rightOperand);

			if (!binOperNode)
			{
				assert(!"Error");
				return nullptr;
			}

			ExpressionNode* exprNode = AstCreateExpressionNode(ast, binOperNode);

			if (!exprNode)
			{
				assert(!"Error");
				return nullptr;
			}

			leftOperand = exprNode;
		}
		else
			leftOperand = rightOperand;

		if (ELEM(parser->CurrentToken)->Type == LngTokenType::Operator &&
			(ELEM(parser->CurrentToken)->Value.Operator == OperatorType::Addition ||
			 ELEM(parser->CurrentToken)->Value.Operator == OperatorType::Subtraction))
		{
			oper = ELEM(parser->CurrentToken)->Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

ExpressionNode* ParseMulDiv(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	ExpressionNode* leftOperand  = nullptr;
	ExpressionNode* rightOperand = nullptr;
	OperatorType oper = OperatorType::Null;

	do
	{
		rightOperand = ParseUnaryAddSub(parser);

		if (leftOperand)
		{
			BinaryOperatorNode* binOperNode = AstCreateBinaryOperatorNode(ast, oper, leftOperand, rightOperand);

			if (!binOperNode)
			{
				assert(!"Error");
				return nullptr;
			}

			ExpressionNode* exprNode = AstCreateExpressionNode(ast, binOperNode);

			if (!exprNode)
			{
				assert(!"Error");
				return nullptr;
			}

			leftOperand = exprNode;
		}
		else
			leftOperand = rightOperand;

		if (ELEM(parser->CurrentToken)->Type == LngTokenType::Operator &&
			(ELEM(parser->CurrentToken)->Value.Operator == OperatorType::Multiplication ||
			 ELEM(parser->CurrentToken)->Value.Operator == OperatorType::Division))
		{
			oper = ELEM(parser->CurrentToken)->Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

ExpressionNode* ParseUnaryAddSub(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Operator ||
		(ELEM(parser->CurrentToken)->Value.Operator != OperatorType::Addition &&
		 ELEM(parser->CurrentToken)->Value.Operator != OperatorType::Subtraction))
	{
		return ParseParentheses(parser);
	}

	switch (ELEM(parser->CurrentToken)->Value.Operator)
	{
		case OperatorType::Addition:
		{
			// Игнорируем унарный плюс. Он не меняет знак.

			parser->CurrentToken += 1;

			ExpressionNode* exprNode = ParseParentheses(parser);

			if (!exprNode)
			{
				assert(!"Error");
				return nullptr;
			}

			return exprNode;
		}
		case OperatorType::Subtraction:
		{
			// Обрабатываем унарный минус.

			parser->CurrentToken += 1;

			ExpressionNode* operandNode = ParseParentheses(parser);

			if (!operandNode)
			{
				assert(!"Error");
				return nullptr;
			}

			UnaryOperatorNode* unOperNode = AstCreateUnaryOperatorNode(ast, OperatorType::Subtraction, operandNode);

			if (!unOperNode)
			{
				assert(!"Error");
				return nullptr;
			}

			ExpressionNode* exprNode = AstCreateExpressionNode(ast, unOperNode);

			if (!exprNode)
			{
				assert(!"Error");
				return nullptr;
			}

			return exprNode;
		}
		default:
		{
			assert(!"Unknown");
			return nullptr;
		}
	}
}

ExpressionNode* Parse_Num_Var_Funct(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	switch (ELEM(parser->CurrentToken)->Type)
	{
		case LngTokenType::Identifier:
		{
			if (ELEM(parser->CurrentToken + 1)->Type == LngTokenType::SpecialSymbol &&
				ELEM(parser->CurrentToken + 1)->Value.SpecialSymbol == SpecialSymbolType::OpeningParenthesis)
			{

				const int functNameId = ELEM(parser->CurrentToken)->Value.Identifier;
				FunctCallNode* functCallNode = AstCreateFunctCallNode(ast, functNameId);

				if (!functCallNode)
				{
					assert(!"Error");
					return nullptr;
				}

				parser->CurrentToken += 2;

				ProgramStatus status = ParseFunctionCallParams(parser, &functCallNode->Params);

				if (status != ProgramStatus::Ok)
				{
					AstFunctCallNodeDestructor(functCallNode);
					assert(!"Error");
					return nullptr;
				}

				ExpressionNode* exprNode = AstCreateExpressionNode(ast, functCallNode);

				if (!exprNode)
				{
					AstFunctCallNodeDestructor(functCallNode);
					assert(!"Error");
					return nullptr;
				}

				return exprNode;
			}
			else
			{
				int varNameId = ELEM(parser->CurrentToken)->Value.Identifier;
				VariableNode* varNode = AstCreateVariableNode(ast, varNameId, nullptr);

				if (!varNode)
				{
					assert(!"Error");
					return nullptr;
				}

				parser->CurrentToken += 1;

				ExpressionNode* exprNode = AstCreateExpressionNode(ast, varNode);

				if (!exprNode)
				{
					assert(!"Error");
					return nullptr;
				}

				return exprNode;
			}
			break;
		}
		case LngTokenType::Number:
		{
			double number = ELEM(parser->CurrentToken)->Value.Number;
			ExpressionNode* exprNode = AstCreateExpressionNode(ast, number);

			if (!exprNode)
			{
				assert(!"Error");
				return nullptr;
			}

			parser->CurrentToken += 1;

			return exprNode;
		}
		default:
			break;
	}

	assert(!"Error");
	return nullptr;
}

ExpressionNode* ParseParentheses(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;

	if (ELEM(parser->CurrentToken)->Type == LngTokenType::SpecialSymbol &&
		ELEM(parser->CurrentToken)->Value.SpecialSymbol == SpecialSymbolType::OpeningParenthesis)
	{
		parser->CurrentToken += 1;

		ExpressionNode* node = ParseExpression(parser);
		if (!node)
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

		parser->CurrentToken += 1;

		return node;
	}
	else
	{
		return Parse_Num_Var_Funct(parser);
	}
}

VariableNode* ParseVariable(Parser* const parser)
{
	assert(parser);

	const ExtArray* const tokens = parser->Tokens;
	AST* ast = parser->OutAst;

	if (ELEM(parser->CurrentToken)->Type != LngTokenType::Identifier)
	{
		return nullptr;
	}

	if (ELEM(parser->CurrentToken + 1)->Type != LngTokenType::Operator ||
		ELEM(parser->CurrentToken + 1)->Value.Operator != OperatorType::SetValue)
	{
		return nullptr;
	}

	// "Идентификатор" + "=" => объявление переменной.

	int varNameId = ELEM(parser->CurrentToken)->Value.Identifier;

	parser->CurrentToken += 2;

	ExpressionNode* exprNode = ParseExpression(parser);
	if (!exprNode)
	{
		assert(!"Error");
		return nullptr;
	}

	VariableNode* node = AstCreateVariableNode(ast, varNameId, exprNode);
	if (!node)
	{
		assert(!"Error");
		return nullptr;
	}

	return node;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !PARSE_EXPR_H