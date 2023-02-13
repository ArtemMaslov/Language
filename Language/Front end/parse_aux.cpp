///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль синтаксического анализатора. Функции поддержки языковых конструкций.
// 
// Файл с исходным кодом.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 14:29 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef PARSE_AUXILIARY_H
#define PARSE_AUXILIARY_H

#include <assert.h>

#include "../Modules/Logs/Logs.h"

#include "Parser.h"
#include "parser_private.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define CREATE_AND_INIT_INSTR_NODE(node)							  \
	InstructionNode* instrNode = AstCreateInstructionNode(ast, node); \
	if (!instrNode)													  \
	{																  \
		TRACE_ERROR();												  \
		return ProgramStatus::Fault;								  \
	}																  \
																	  \
	if (ExtArrayAddElem(instrBlock, instrNode) != ProgramStatus::Ok)  \
	{																  \
		TRACE_ERROR();												  \
		return ProgramStatus::Fault;								  \
	}

ProgramStatus ParseProgramBlock(Parser* const parser, ExtArray* instrBlock)
{
	assert(parser);
	assert(instrBlock);

	const ExtArray* const tokens = parser->Tokens;
	AST* const ast = parser->OutAst;
	
	if (ELEM(parser->CurrentToken)->Type != LngTokenType::SpecialSymbol ||
		ELEM(parser->CurrentToken)->Value.SpecialSymbol != SpecialSymbolType::OpeningBrace)
	{
		assert(!"Error");
		return ProgramStatus::Fault;
	}

	parser->CurrentToken++;

	while (ELEM(parser->CurrentToken)->Type != LngTokenType::SpecialSymbol ||
		   ELEM(parser->CurrentToken)->Value.SpecialSymbol != SpecialSymbolType::ClosingBrace)
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