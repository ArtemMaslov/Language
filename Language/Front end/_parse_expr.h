#ifndef PARSE_EXPR_H
#define PARSE_EXPR_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static ExpressionNode* ParseExpression(Parser* parser)
{
	assert(parser);

	ExpressionNode* exprNode = ParseOr(parser);

	return exprNode;
}

static ExpressionNode* ParseOr(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

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

		if (tokens[parser->CurrentToken].Type == LngTokenType::Operator &&
			(tokens[parser->CurrentToken].Value.Operator == OperatorType::Or))
		{
			oper = tokens[parser->CurrentToken].Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

static ExpressionNode* ParseAnd(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

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

		if (tokens[parser->CurrentToken].Type == LngTokenType::Operator &&
			(tokens[parser->CurrentToken].Value.Operator == OperatorType::And))
		{
			oper = tokens[parser->CurrentToken].Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

static ExpressionNode* ParseNot(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Operator ||
		tokens[parser->CurrentToken].Value.Operator != OperatorType::Not)
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
static ExpressionNode* ParseEqual(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

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

		if (tokens[parser->CurrentToken].Type == LngTokenType::Operator &&
			(tokens[parser->CurrentToken].Value.Operator == OperatorType::Equal ||
			 tokens[parser->CurrentToken].Value.Operator == OperatorType::NotEqual))
		{
			oper = tokens[parser->CurrentToken].Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

/// >, <, >=, <=
static ExpressionNode* ParseCompare(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

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

		if (tokens[parser->CurrentToken].Type == LngTokenType::Operator &&
			(tokens[parser->CurrentToken].Value.Operator == OperatorType::Greater ||
			 tokens[parser->CurrentToken].Value.Operator == OperatorType::GreaterEqual ||
			 tokens[parser->CurrentToken].Value.Operator == OperatorType::Less ||
			 tokens[parser->CurrentToken].Value.Operator == OperatorType::LessEqual))
		{
			oper = tokens[parser->CurrentToken].Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

static ExpressionNode* ParseAddSub(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

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

		if (tokens[parser->CurrentToken].Type == LngTokenType::Operator &&
			(tokens[parser->CurrentToken].Value.Operator == OperatorType::Addition ||
			 tokens[parser->CurrentToken].Value.Operator == OperatorType::Subtraction))
		{
			oper = tokens[parser->CurrentToken].Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

static ExpressionNode* ParseMulDiv(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

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

		if (tokens[parser->CurrentToken].Type == LngTokenType::Operator &&
			(tokens[parser->CurrentToken].Value.Operator == OperatorType::Multiplication ||
			 tokens[parser->CurrentToken].Value.Operator == OperatorType::Division))
		{
			oper = tokens[parser->CurrentToken].Value.Operator;
			parser->CurrentToken += 1;
		}
		else
			break;
	}
	while (rightOperand);

	return leftOperand;
}

static ExpressionNode* ParseUnaryAddSub(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Operator ||
		(tokens[parser->CurrentToken].Value.Operator != OperatorType::Addition &&
		 tokens[parser->CurrentToken].Value.Operator != OperatorType::Subtraction))
	{
		return ParseParentheses(parser);
	}

	switch (tokens[parser->CurrentToken].Value.Operator)
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

static ExpressionNode* Parse_Num_Var_Funct(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	switch (tokens[parser->CurrentToken].Type)
	{
		case LngTokenType::Identifier:
		{
			if (tokens[parser->CurrentToken + 1].Type == LngTokenType::SpecialSymbol &&
				tokens[parser->CurrentToken + 1].Value.SpecialSymbol == SpecialSymbolType::OpeningParenthesis)
			{
				//***///***///---\\\***\\\***\\\___/// Вызов функции \\\___///***///***///---\\\***\\\***\\ 

				const int functNameId = tokens[parser->CurrentToken].Value.Identifier;
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
				//***///***///---\\\***\\\***\\\___/// Переменная \\\___///***///***///---\\\***\\\***\\ 

				int varNameId = tokens[parser->CurrentToken].Value.Identifier;
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
			double number = tokens[parser->CurrentToken].Value.Number;
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

static ExpressionNode* ParseParentheses(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type == LngTokenType::SpecialSymbol &&
		tokens[parser->CurrentToken].Value.SpecialSymbol == SpecialSymbolType::OpeningParenthesis)
	{
		parser->CurrentToken += 1;

		ExpressionNode* node = ParseExpression(parser);
		if (!node)
		{
			assert(!"Error");
			return nullptr;
		}

		if (tokens[parser->CurrentToken].Type != LngTokenType::SpecialSymbol ||
			tokens[parser->CurrentToken].Value.SpecialSymbol != SpecialSymbolType::ClosingParenthesis)
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

static VariableNode* ParseVariable(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Identifier)
	{
		return nullptr;
	}

	if (tokens[parser->CurrentToken + 1].Type != LngTokenType::Operator ||
		tokens[parser->CurrentToken + 1].Value.Operator != OperatorType::SetValue)
	{
		return nullptr;
	}

	// "Идентификатор" + "=" => объявление переменной.

	int varNameId = tokens[parser->CurrentToken].Value.Identifier;

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