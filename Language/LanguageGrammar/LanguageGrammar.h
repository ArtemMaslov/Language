#ifndef LANGUAGE_GRAMMAR_H
#define LANGUAGE_GRAMMAR_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

// �������� �����
enum KeywordTypes
{
	LNG_KWD_IF         = 0,
	LNG_KWD_ELSE       = 1,

	LNG_KWD_WHILE      = 10,

	LNG_KWD_RET        = 20,

	LNG_KWD_IN         = 30,
	LNG_KWD_OUT        = 31,

};

// ���������
enum OperatorTypes
{
	// �������������� ��������.
	LNG_OPER_ADD        = 0,  // ��������.
	LNG_OPER_SUB        = 1,  // ���������.
	LNG_OPER_MUL        = 2,  // ���������.
	LNG_OPER_DIV        = 3,  // �������.

	// ��������� ���������.
	LNG_OPER_EQUAL      = 100, // �������� ��������� �����.
	LNG_OPER_NOT_EQUAL  = 101, // �������� ��������� �� �����.

	//LNG_MOP_GREATER_EQUAL
	LNG_OPER_GREATER    = 103, // �������� ��������� ������.

	//LNG_MOP_LESS_EQUAL
	LNG_OPER_LESS       = 105, // �������� ��������� ������.

    // ���������� ���������.
	LNG_OPER_NOT        = 120, // ���������� ��.
	LNG_OPER_AND        = 121, // ���������� �.
	LNG_OPER_OR         = 122, // ���������� ���.

	LNG_OPER_SET_VALUE  = 130, // ����������.
};

// ����������� �������.
enum SpecialSymbolTypes
{
	LNG_SPEC_L_ROUND_BRACKET  = '(',  // ������ ������������� ������� ������ (.
	LNG_SPEC_R_ROUND_BRACKET  = ')',  // ������ ������������� ������� ������ ).

	LNG_SPEC_L_SQUARE_BRACKET = '[',  // ������ ������������� ���������� ������ [.
	LNG_SPEC_R_SQUARE_BRACKET = ']',  // ������ ������������� ���������� ������ ].

	LNG_SPEC_L_CURLY_BRACKET  = '{',  // ������ ������������� �������� ������ {.
	LNG_SPEC_R_CURLY_BRACKET  = '}',  // ������ ������������� �������� ������ }.

	LNG_SPEC_POINT            = '.',  // ������ ����� ..
	LNG_SPEC_COMMA            = ',',  // ������ ������� ,.

	//LNG_SPEC_EQUAL            = '=',  // ������ ����� =.
	LNG_SPEC_COLON            = ':',  // ������ ��������� :.
	LNG_SPEC_SEMICOLON        = ';',  // ������ ����� � ������� ;.

	LNG_SPEC_SINGLE_QUOTES    = '\'', // ������ ��������� ������� '.
	LNG_SPEC_DOUBLE_QUOTES    = '\"', // ������ ������� ������� ".
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

// ���� ������
enum TokenTypes
{
	LNG_KEYWORD        = 0,
	LNG_OPERATOR       = 1,
	LNG_IDENTIFIER     = 2,
	LNG_SPECIAL_SYMBOL = 3
};

enum GrammarType
{
	LNG_GRAMMAR_UNI = 1 << 0, // ������������� ����������.
	LNG_GRAMMAR_RU  = 1 << 1, // ������� ��� ����������.
	LNG_GRAMMAR_EN  = 1 << 2, // ���������� ��� ����������.
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