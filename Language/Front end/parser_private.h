///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль синтаксического анализатора. Файлы приватной конфигурации.
// 
// Версия: 1.0.0.0
// Дата последнего изменения: 14:26 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef PARSER_PRIVATE_H
#define PARSER_PRIVATE_H

#include "../Modules/ExtArray/ExtArray.h"
#include "../AST/AST.h"

#define ELEM(index) \
	((Token*)ExtArrayGetElemAt(tokens, index))

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Структура синтаксического анализатора.
struct Parser
{
	/**
	* @brief Массив лексем.
	* ExtArray<Token>. 
	*/
	const ExtArray* const Tokens;
	/// Текущая лексема.
	size_t                CurrentToken;
	/// Количество лексем.
	const size_t          TokensCount;

	/// Выходное абстрактное синтаксическое дерево.
	AST* const            OutAst;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ExpressionNode* ParseExpression(Parser* const parser);

ExpressionNode* ParseOr(Parser* const parser);

ExpressionNode* ParseAnd(Parser* const parser);

ExpressionNode* ParseNot(Parser* const parser);

ExpressionNode* ParseEqual(Parser* const parser);

ExpressionNode* ParseCompare(Parser* const parser);

ExpressionNode* ParseAddSub(Parser* const parser);

ExpressionNode* ParseMulDiv(Parser* const parser);

ExpressionNode* ParseUnaryAddSub(Parser* const parser);

ExpressionNode* Parse_Num_Var_Funct(Parser* const parser);

ExpressionNode* ParseParentheses(Parser* const parser);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ExpressionNode* ParseCondition(Parser* const parser);

FunctCallNode* ParseFunctionCall(Parser* const parser);

ProgramStatus ParseFunctionCallParams(Parser* const parser, ExtArray* const outParams);

ProgramStatus ParseFunctionDefParams(Parser* const parser, ExtArray* const outParams);

FunctDefNode* ParseFunctionDef(Parser* const parser);

VariableNode* ParseVariable(Parser* const parser);

InputNode* ParseInput(Parser* const parser);

OutputNode* ParseOutput(Parser* const parser);

ReturnNode* ParseReturn(Parser* const parser);

WhileNode* ParseWhile(Parser* const parser);

IfNode* ParseIf(Parser* const parser);

ProgramStatus ParseProgramBlock(Parser* const parser, ExtArray* const instrBlock);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !PARSER_PRIVATE_H
