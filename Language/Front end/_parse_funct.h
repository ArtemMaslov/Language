#ifndef PARSE_FUNCT_H
#define PARSE_FUNCT_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static FunctCallNode* ParseFunctionCall(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Identifier)
	{
		return nullptr;
	}

	if (tokens[parser->CurrentToken + 1].Type != LngTokenType::SpecialSymbol ||
		tokens[parser->CurrentToken + 1].Value.SpecialSymbol != SpecialSymbolType::OpeningParenthesis)
	{
		return nullptr;
	}

	const int functNameId = tokens[parser->CurrentToken].Value.Identifier;
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

static ProgramStatus ParseFunctionCallParams(Parser* parser, ExtArray* outParams)
{
	assert(parser);
	assert(outParams);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	while (tokens[parser->CurrentToken].Type != LngTokenType::SpecialSymbol ||
		   tokens[parser->CurrentToken].Value.SpecialSymbol != SpecialSymbolType::ClosingParenthesis)
	{
		ExpressionNode* expr = ParseExpression(parser);
		if (!expr)
		{
			assert(!"Error");
			return ProgramStatus::Error;
		}

		FunctParamNode* param = AstCreateFunctParamNode(ast, expr);
		if (!param)
		{
			assert(!"Error");
			return ProgramStatus::Fault;
		}
		ExtArrayAddElem(outParams, param);

		if (tokens[parser->CurrentToken].Type == LngTokenType::SpecialSymbol &&
			tokens[parser->CurrentToken].Value.SpecialSymbol == SpecialSymbolType::Comma)
		{
			parser->CurrentToken++;
		}
		else if (tokens[parser->CurrentToken].Type == LngTokenType::SpecialSymbol &&
				 tokens[parser->CurrentToken].Value.SpecialSymbol == SpecialSymbolType::ClosingParenthesis)
		{
			break;
		}
		else
		{
			assert(!"Error");
			return ProgramStatus::Error;
		}
	}

	// Обрабатываем закрывающуюся круглую скобку ")".
	parser->CurrentToken++;

	return ProgramStatus::Ok;
}

static ProgramStatus ParseFunctionDefParams(Parser* parser, ExtArray* outParams)
{
	assert(parser);
	assert(outParams);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	while (tokens[parser->CurrentToken].Type != LngTokenType::SpecialSymbol ||
		   tokens[parser->CurrentToken].Value.SpecialSymbol != SpecialSymbolType::ClosingParenthesis)
	{
		if (tokens[parser->CurrentToken].Type != LngTokenType::Identifier)
		{
			assert(!"Error");
			return ProgramStatus::Error;
		}

		int nameId = tokens[parser->CurrentToken].Value.Identifier;
		FunctParamNode* param = AstCreateFunctParamNode(ast, nameId);
		if (!param)
		{
			assert(!"Error");
			return ProgramStatus::Fault;
		}
		ExtArrayAddElem(outParams, param);

		parser->CurrentToken++;

		if (tokens[parser->CurrentToken].Type == LngTokenType::SpecialSymbol &&
			tokens[parser->CurrentToken].Value.SpecialSymbol == SpecialSymbolType::Comma)
		{
			parser->CurrentToken++;
		}
		else if (tokens[parser->CurrentToken].Type == LngTokenType::SpecialSymbol &&
				 tokens[parser->CurrentToken].Value.SpecialSymbol == SpecialSymbolType::ClosingParenthesis)
		{
			break;
		}
		else
		{
			assert(!"Error");
			return ProgramStatus::Error;
		}
	}

	// Обрабатываем закрывающуюся круглую скобку ")".
	parser->CurrentToken++;

	return ProgramStatus::Ok;
}

static FunctDefNode* ParseFunctionDef(Parser* parser)
{
	assert(parser);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::Identifier)
	{
		return nullptr;
	}

	if (tokens[parser->CurrentToken + 1].Type != LngTokenType::SpecialSymbol ||
		tokens[parser->CurrentToken + 1].Value.SpecialSymbol != SpecialSymbolType::OpeningParenthesis)
	{
		return nullptr;
	}

	// "Идентификатор" + "(" => объявление функции.

	const int functNameId = tokens[parser->CurrentToken].Value.Identifier;
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

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !PARSE_FUNCT_H