#ifndef LANGUAGE_GRAMMAR_H
#define LANGUAGE_GRAMMAR_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

// �������� �����
enum class KeywordType
{
	Null   = -1, // ������� �� �������� �������� ������.

	If     = 0,
	Else   = 1,

	While  = 10,

	Return = 20,

	Input  = 30,
	Output = 31,

};

// ���������
enum class OperatorType
{
	Null			= -1,  // ������� �� �������� ����������.

	// �������������� ��������.
	Addition        = 0,   // ��������.
	Subtraction     = 1,   // ���������.
	Multiplication  = 2,   // ���������.
	Division        = 3,   // �������.

	// ��������� ���������.
	Equal           = 100, // �������� ��������� �����.
	NotEqual        = 101, // �������� ��������� �� �����.

	GreaterEqual    = 102, // �������� ��������� ������ ��� �����.
	Greater         = 103, // �������� ��������� ������.

	LessEqual       = 104, // �������� ��������� ������ ��� �����.
	Less            = 105, // �������� ��������� ������.

    // ���������� ���������.
	Not             = 120, // ���������� ��.
	And             = 121, // ���������� �.
	Or              = 122, // ���������� ���.

	SetValue        = 130, // ����������.
};

// ����������� �������.
enum class SpecialSymbolType
{
	Null             = -1,   // ������� �� �������� ����������� ��������.

	OpeningParenthesis  = '(',  // ������ ������������� ������� ������ (.
	ClosingParenthesis  = ')',  // ������ ������������� ������� ������ ).

	OpeningSquareBracket = '[',  // ������ ������������� ���������� ������ [.
	ClosingSquareBracket = ']',  // ������ ������������� ���������� ������ ].

	OpeningBrace  = '{',  // ������ ������������� �������� ������ {.
	ClosingBrace  = '}',  // ������ ������������� �������� ������ }.

	Point            = '.',  // ������ ����� ..
	Comma            = ',',  // ������ ������� ,.

	//Equal            = '=',  // ������ ����� =.
	Colon            = ':',  // ������ ��������� :.
	Semicolon        = ';',  // ������ ����� � ������� ;.

	SingleQuotes    = '\'', // ������ ��������� ������� '.
	DoubleQuotes    = '\"', // ������ ������� ������� ".
};

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

// ���� ������
// �������� LngTokenType, � �� TokenType, ������ ��� TokenType - ��� ���� �� �������� enum ������������ ����������.
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
	Universal = 1 << 0, // ������������� ����������.
	Russian   = 1 << 1, // ������� ��� ����������.
	English   = 1 << 2, // ���������� ��� ����������.
};

struct GrammarToken
{
	LngTokenType   Type;
	GrammarType Grammar;

	union
	{
		// �������� �������� �������. 
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

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const GrammarToken* GrammarGetName(LngTokenType type, int id);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !LANGUAGE_GRAMMAR_H