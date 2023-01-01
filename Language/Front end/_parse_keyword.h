#ifndef PARSE_KEYWORD_H
#define PARSE_KEYWORD_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static IfNode* ParseIf(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Keyword ||
		tokens[parser->CurrentToken].Value.Keyword != KeywordType::If)
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

	if (tokens[parser->CurrentToken].Type == LngTokenType::Keyword &&
		tokens[parser->CurrentToken].Value.Keyword == KeywordType::Else)
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

static InputNode* ParseInput(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Keyword ||
		tokens[parser->CurrentToken].Value.Keyword != KeywordType::Input)
	{
		return nullptr;
	}

	parser->CurrentToken += 1;

	//VariableNode* varNode = ParseVariable(parser);

	if (tokens[parser->CurrentToken].Type != LngTokenType::Identifier)
	{
		assert(!"Error");
		return nullptr;
	}

	int nameId = tokens[parser->CurrentToken].Value.Identifier;
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

static OutputNode* ParseOutput(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Keyword ||
		tokens[parser->CurrentToken].Value.Keyword != KeywordType::Output)
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

static ReturnNode* ParseReturn(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Keyword ||
		tokens[parser->CurrentToken].Value.Keyword != KeywordType::Return)
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

static WhileNode* ParseWhile(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Keyword ||
		tokens[parser->CurrentToken].Value.Keyword != KeywordType::While)
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
static ExpressionNode* ParseCondition(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;

	if (tokens[parser->CurrentToken].Type != LngTokenType::SpecialSymbol ||
		tokens[parser->CurrentToken].Value.SpecialSymbol != SpecialSymbolType::OpeningParenthesis)
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

	if (tokens[parser->CurrentToken].Type != LngTokenType::SpecialSymbol ||
		tokens[parser->CurrentToken].Value.SpecialSymbol != SpecialSymbolType::ClosingParenthesis)
	{
		assert(!"Error");
		return nullptr;
	}

	parser->CurrentToken++;

	return conditionNode;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !PARSE_KEYWORD_H