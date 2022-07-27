#ifndef LANGUAGE_GRAMMAR_H
#define LANGUAGE_GRAMMAR_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

// Ключевые слова
enum KeywordTypes
{
	LNG_KWD_IF         = 0,
	LNG_KWD_ELSE       = 1,

	LNG_KWD_WHILE      = 10,

	LNG_KWD_RET        = 20,

	LNG_KWD_IN         = 30,
	LNG_KWD_OUT        = 31,

};

// Операторы
enum OperatorTypes
{
	// Арифметические операции.
	LNG_OPER_ADD        = 0,  // Сложение.
	LNG_OPER_SUB        = 1,  // Вычитание.
	LNG_OPER_MUL        = 2,  // Умножение.
	LNG_OPER_DIV        = 3,  // Деление.

	// Операторы сравнения.
	LNG_OPER_EQUAL      = 100, // Оператор сравнения равно.
	LNG_OPER_NOT_EQUAL  = 101, // Оператор сравнения не равно.

	//LNG_MOP_GREATER_EQUAL
	LNG_OPER_GREATER    = 103, // Оператор сравнения больше.

	//LNG_MOP_LESS_EQUAL
	LNG_OPER_LESS       = 105, // Оператор сравнения меньше.

    // Логические операторы.
	LNG_OPER_NOT        = 120, // Логическое не.
	LNG_OPER_AND        = 121, // Логическое и.
	LNG_OPER_OR         = 122, // Логическое или.

	LNG_OPER_SET_VALUE  = 130, // Присвоение.
};

// Специальные символы.
enum SpecialSymbolTypes
{
	LNG_SPEC_L_ROUND_BRACKET  = '(',  // Символ открывающаяся круглая скобка (.
	LNG_SPEC_R_ROUND_BRACKET  = ')',  // Символ закрывающаяся круглая скобка ).

	LNG_SPEC_L_SQUARE_BRACKET = '[',  // Символ открывающаяся квадратная скобка [.
	LNG_SPEC_R_SQUARE_BRACKET = ']',  // Символ закрывающаяся квадратная скобка ].

	LNG_SPEC_L_CURLY_BRACKET  = '{',  // Символ открывающаяся фигурная скобка {.
	LNG_SPEC_R_CURLY_BRACKET  = '}',  // Символ закрывающаяся фигурная скобка }.

	LNG_SPEC_POINT            = '.',  // Символ точка ..
	LNG_SPEC_COMMA            = ',',  // Символ запятая ,.

	//LNG_SPEC_EQUAL            = '=',  // Символ равно =.
	LNG_SPEC_COLON            = ':',  // Символ двоеточие :.
	LNG_SPEC_SEMICOLON        = ';',  // Символ точка с запятой ;.

	LNG_SPEC_SINGLE_QUOTES    = '\'', // Символ одинарная кавычка '.
	LNG_SPEC_DOUBLE_QUOTES    = '\"', // Символ двойная кавычка ".
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

// Типы лексем
enum TokenTypes
{
	LNG_KEYWORD        = 0,
	LNG_OPERATOR       = 1,
	LNG_IDENTIFIER     = 2,
	LNG_SPECIAL_SYMBOL = 3
};

enum GrammarType
{
	LNG_GRAMMAR_UNI = 1 << 0, // Универсальная грамматика.
	LNG_GRAMMAR_RU  = 1 << 1, // Русский тип грамматики.
	LNG_GRAMMAR_EN  = 1 << 2, // Английский тип грамматики.
};

struct GrammarToken
{
	TokenTypes  Type;
	GrammarType Grammar;

	union
	{
		KeywordTypes       Keyword;
		OperatorTypes      Operator;
		SpecialSymbolTypes SpecialSymbol;
	};

	const char* Name;
};

extern GrammarToken Operators[];
extern GrammarToken SpecialSymbols[];
extern GrammarToken Keywords[];

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#endif