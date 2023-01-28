#ifndef PARSE_AUXILIARY_H
#define PARSE_AUXILIARY_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define CREATE_AND_INIT_INSTR_NODE(node)							  \
	InstructionNode* instrNode = AstCreateInstructionNode(ast, node); \
	if (!instrNode)													  \
	{																  \
		assert(!"Error");											  \
		return ProgramStatus::Fault;								  \
	}																  \
																	  \
	if (ExtArrayAddElem(instrBlock, instrNode) != ProgramStatus::Ok)  \
	{																  \
		assert(!"Error");											  \
		return ProgramStatus::Fault;								  \
	}

static ProgramStatus ParseProgramBlock(Parser* parser, ExtArray* instrBlock)
{
	assert(parser);
	assert(instrBlock);

	const Token* tokens = parser->Commands;
	AST* ast = parser->outAst;

	if (tokens[parser->CurrentToken].Type != LngTokenType::SpecialSymbol ||
		tokens[parser->CurrentToken].Value.SpecialSymbol != SpecialSymbolType::OpeningBrace)
	{
		assert(!"Error");
		return ProgramStatus::Error;
	}

	parser->CurrentToken++;

	while (tokens[parser->CurrentToken].Type != LngTokenType::SpecialSymbol ||
		   tokens[parser->CurrentToken].Value.SpecialSymbol != SpecialSymbolType::ClosingBrace)
	{
		VariableNode* varNode = ParseVariable(parser);
		if (varNode)
		{
			CREATE_AND_INIT_INSTR_NODE(varNode);
			continue;
		}

		WhileNode* whileNode = ParseWhile(parser);
		if (whileNode)
		{
			CREATE_AND_INIT_INSTR_NODE(whileNode);
			continue;
		}

		IfNode* ifNode = ParseIf(parser);
		if (ifNode)
		{
			CREATE_AND_INIT_INSTR_NODE(ifNode);
			continue;
		}

		InputNode* inputNode = ParseInput(parser);
		if (inputNode)
		{
			CREATE_AND_INIT_INSTR_NODE(inputNode);
			continue;
		}

		OutputNode* outputNode = ParseOutput(parser);
		if (outputNode)
		{
			CREATE_AND_INIT_INSTR_NODE(outputNode);
			continue;
		}

		FunctCallNode* functCallNode = ParseFunctionCall(parser);
		if (functCallNode)
		{
			CREATE_AND_INIT_INSTR_NODE(functCallNode);
			continue;
		}

		ReturnNode* retNode = ParseReturn(parser);
		if (retNode)
		{
			CREATE_AND_INIT_INSTR_NODE(retNode);
			continue;
		}

		assert(!"Error");
		return ProgramStatus::Fault;
	}

	// Обрабатываем закрывающуюся фигурную скобку "}".
	parser->CurrentToken += 1;

	return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !PARSE_AUXILIARY_H