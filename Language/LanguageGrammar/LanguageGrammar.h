#ifndef LANGUAGE_GRAMMAR_H
#define LANGUAGE_GRAMMAR_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

// Ключевые слова
enum class KeywordType
{
	Null   = -1, // Лексема не является ключевым словом.

	If     = 0,
	Else   = 1,

	While  = 10,

	Return = 20,

	Input  = 30,
	Output = 31,

};

// Операторы
enum class OperatorType
{
	Null			= -1,  // Лексема не является оператором.

	// Арифметические операции.
	Addition        = 0,   // Сложение.
	Subtraction     = 1,   // Вычитание.
	Multiplication  = 2,   // Умножение.
	Division        = 3,   // Деление.

	// Операторы сравнения.
	Equal           = 100, // Оператор сравнения равно.
	NotEqual        = 101, // Оператор сравнения не равно.

	GreaterEqual    = 102, // Оператор сравнения больше или равно.
	Greater         = 103, // Оператор сравнения больше.

	LessEqual       = 104, // Оператор сравнения меньше или равно.
	Less            = 105, // Оператор сравнения меньше.

    // Логические операторы.
	Not             = 120, // Логическое не.
	And             = 121, // Логическое и.
	Or              = 122, // Логическое или.

	SetValue        = 130, // Присвоение.
};

// Специальные символы.
enum class SpecialSymbolType
{
	Null             = -1,   // Лексема не является специальным символом.

	OpeningParenthesis  = '(',  // Символ открывающаяся круглая скобка (.
	ClosingParenthesis  = ')',  // Символ закрывающаяся круглая скобка ).

	OpeningSquareBracket = '[',  // Символ открывающаяся квадратная скобка [.
	ClosingSquareBracket = ']',  // Символ закрывающаяся квадратная скобка ].

	OpeningBrace  = '{',  // Символ открывающаяся фигурная скобка {.
	ClosingBrace  = '}',  // Символ закрывающаяся фигурная скобка }.

	Point            = '.',  // Символ точка ..
	Comma            = ',',  // Символ запятая ,.

	//Equal            = '=',  // Символ равно =.
	Colon            = ':',  // Символ двоеточие :.
	Semicolon        = ';',  // Символ точка с запятой ;.

	SingleQuotes    = '\'', // Символ одинарная кавычка '.
	DoubleQuotes    = '\"', // Символ двойная кавычка ".
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

// Типы лексем
// Название LngTokenType, а не TokenType, потому что TokenType - это одно из значений enum подключаемой библиотеки.
enum class LngTokenType
{
	Keyword       = 0,
	Operator      = 1,
	Identifier    = 2,
	SpecialSymbol = 3,
	Number        = 4
};

enum class GrammarType
{
	Universal = 1 << 0, // Универсальная грамматика.
	Russian   = 1 << 1, // Русский тип грамматики.
	English   = 1 << 2, // Английский тип грамматики.
};

struct GrammarToken
{
	LngTokenType   Type;
	GrammarType Grammar;

	union
	{
		// Числовое значение лексемы. 
		int               Int;
		KeywordType       Keyword;
		OperatorType      Operator;
		SpecialSymbolType SpecialSymbol;
	} Value;

	const char*  Name;
	const size_t NameSize;
};

extern GrammarToken Operators[];
extern GrammarToken SpecialSymbols[];
extern GrammarToken Keywords[];

extern const size_t KeywordsSize;
extern const size_t SpecialSymbolsSize;
extern const size_t OperatorsSize;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

const GrammarToken* GrammarGetName(LngTokenType type, int id);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !LANGUAGE_GRAMMAR_H