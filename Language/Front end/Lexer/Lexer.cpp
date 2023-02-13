///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль лексического анализатора.
// 
// Файлы с исходным кодом.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 16:17 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../../Modules/Logs/Logs.h"
#include "../../Modules/ErrorsHandling.h"

#include "Lexer.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Выполнить лексический анализ текста.
 * 
 * @param lexer Указатель на структуру лексического анализатора. Содержит результаты 
 *              работы функции.
 * @param text  Указатель структуру исходного текста программы.
 * 
 * @return ProgramStatus::Fault, если в процессе анализа текста была встречена лексическая ошибка.
 *         ProgramStatus::Ok, если нет ошибок.
*/
static LexerError LexerGetTokens(Lexer* const lexer, Text* const text);

/**
 * @brief  Пытается прочитать лексему как идентификатор или ключевое слово.
 * 
 * @param text  Указатель на начало лексемы.
 * @param lexer Указатель на структуру лексического анализатора.
 * 
 * @return LexerError::AddToken, если лексема была прочитана как идентификатор, но возникла
 *         ошибка при добавлении её в массив лексем.
 *         LexerError::NotIdentifier, если лексема не является идентификатором.
 *         LexerError::IdentifierRead, если лексема является идентификатором, и нет ошибок.
*/
static LexerError TryReadIdentifier(const char** const text, Lexer* const lexer);

/**
 * @brief  Проверяет, является ли идентификатор ключевым словом.
 * 
 * @param name       Указатель на начало лексемы.
 * @param nameLength Длина лексемы.
 * 
 * @return KeywordType::Null, если лексема не является ключевым словом.
 *         Если лексема является ключевым словом, то его тип.
*/
static KeywordType CheckKeyword(const char* const name, const size_t nameLength);

/**
 * @brief  Пытается прочитать лексему как оператор.
 * 
 * @param text   Указатель на начало лексемы.
 * @param tokens Указатель на массив лексем.
 * 
 * @return LexerError::AddToken, если лексема была прочитана как оператор, но возникла
 *         ошибка при добавлении её в массив лексем.
 *         LexerError::NotOperator, если лексема не является оператором.
 *         LexerError::Operator, если лексема является оператором, и нет ошибок.
*/
static LexerError TryReadOperator(const char** const text, ExtArray* const tokens);

/**
 * @brief  Пытается прочитать лексему как специальный символ.
 * 
 * @param text   Указатель на начало лексемы.
 * @param tokens Указатель на массив лексем.
 * 
 * @return LexerError::AddToken, если лексема была прочитана как специальный символ, но возникла
 *         ошибка при добавлении её в массив лексем.
 *         LexerError::NotSpecSymbol, если лексема не является специальным символом.
 *         LexerError::SpecSymbolRead, если лексема является специальным символом, и нет ошибок.
*/
static LexerError TryReadSpecSymbol(const char** const text, ExtArray* const tokens);

/**
 * @brief  Пытается прочитать лексему как число.
 * 
 * @param text   Указатель на начало лексемы.
 * @param tokens Указатель на массив лексем.
 * 
 * @return LexerError::AddToken, если лексема была прочитана как число, но возникла
 *         ошибка при добавлении её в массив лексем.
 *         LexerError::NotNumber, если лексема не является числом.
 *         LexerError::NumberRead, если лексема является числом, и нет ошибок.
*/
static LexerError TryReadNumber(const char** const text, ExtArray* const tokens);

/**
 * @brief  Пропускает пробельные символы (' ', '\t', '\n') и увеличивает *text до первого не пробельного символа.
 * 
 * @param text Двойной указатель на анализируемый текст.
*/
static void SkipSpaceSymbols(const char** const text);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

LexerError LexerConstructor(Lexer* const lexer, IdentifierTable* const table)
{
	assert(lexer);
	assert(table);
		
	ProgramStatus status = ExtArrayConstructor(&lexer->Tokens, sizeof(Token));
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return LexerError::ExtArray;
	}

	lexer->Identifiers = table;

	return LexerError::NoErrors;
}

void LexerDestructor(Lexer* const lexer)
{
	assert(lexer);

	// Таблица идентификаторов является внешней для модуля лексического анализатора.
	//IdentifierTableDestructor(lexer->Identifiers);

	ExtArrayDestructor(&lexer->Tokens);
	memset(lexer, 0, sizeof(Lexer));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

LexerError LexerAnalyseFile(Lexer* const lexer, const char* const fileName)
{
	assert(lexer);
	assert(fileName);

	Text text = {};

	if (TextConstructor(&text, fileName) != TextError::NoErrors)
	{
		TRACE_ERROR();
		return LexerError::Text;
	}

	LexerError status = LexerGetTokens(lexer, &text);
	if (status != LexerError::NoErrors)
	{
		TRACE_ERROR();
		TextDestructor(&text);
		return status;
	}

	TextDestructor(&text);

	return LexerError::NoErrors;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static LexerError LexerGetTokens(Lexer* const lexer, Text* const text)
{
	assert(lexer);
	assert(text);

	LexerError status  = LexerError::NoErrors;
	const char* buffer = text->Data;
	ExtArray* const tokens = &lexer->Tokens;

	while (*buffer)
	{
		SkipSpaceSymbols(&buffer);
		if (*buffer == '\0')
			break;

		status = TryReadNumber(&buffer, tokens);
		if (status == LexerError::AddToken)
		{
			TRACE_ERROR();
			return status;
		}
		if (status == LexerError::NumberRead)
			continue;
		// Лексема не является числом.

		status = TryReadIdentifier(&buffer, lexer);
		if (status == LexerError::AddToken)
		{
			TRACE_ERROR();
			return status;
		}
		if (status == LexerError::IdentifierRead)
			continue;
		// Лексема не является идентификатором.

		status = TryReadOperator(&buffer, tokens);
		if (status == LexerError::AddToken)
		{
			TRACE_ERROR();
			return status;
		}
		if (status == LexerError::OperatorRead)
			continue;
		// Лексема не является оператором.
		
		status = TryReadSpecSymbol(&buffer, tokens);
		if (status == LexerError::AddToken)
		{
			TRACE_ERROR();
			return status;
		}
		if (status == LexerError::SpecSymbolRead)
			continue;
		// Лексема не является специальным символом.

		// Не удалось обработать лексему. Выдаём ошибку.
		LOG_ERR("Не известный тип лексемы.");
		ExtArrayDump(&lexer->Tokens);
		LexerLogDump(lexer);

		return LexerError::TokenError;
	}

	return LexerError::NoErrors;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

void LexerLogDump(const Lexer* const lexer)
{
	FILE* const file = LogBeginDump(LogSignature::General, LogLevel::Debug);

	char buffer[LexerDumpBufferSize] = "";

	fputs("Срез состояния лексера.\n", file);

	const size_t tokensCount = lexer->Tokens.Size;
	for (size_t st = 0; st < tokensCount; st++)
	{
		const Token* token = (Token*)ExtArrayGetElemAt(&lexer->Tokens, st);

		switch (token->Type)
		{
			case LngTokenType::Keyword:
			{
				const GrammarToken* const grammarToken = GrammarGetToken(token->Value.Keyword);
				sprintf(buffer, "Keyword:\n%5zd:\t\t\"%s\"\n\n", st, grammarToken->Name);
				break;
			}
			case LngTokenType::Identifier:
			{
				const Identifier* const id = IdentifierGetById(lexer->Identifiers, token->Value.Identifier);
				if (id == nullptr)
				{
					sprintf(buffer, "Identifier:\n%5zd:\t\tIdentifier = %zd не найден!", st, token->Value.Identifier);
					break;
				}
				
				sprintf(buffer, "Identifier:\n%5zd:\t\t\"%s\"\n\n", st, id->Name);
				break;
			}
			case LngTokenType::Number:
			{
				sprintf(buffer, "Number:\n%5zd:\t\t\"%lf\"\n\n", st, token->Value.Number);
				break;
			}
			case LngTokenType::SpecialSymbol:
			{
				const GrammarToken* const grammarToken = GrammarGetToken(token->Value.SpecialSymbol);
				sprintf(buffer, "SpecSym:\n%5zd:\t\t\"%s\"\n\n", st, grammarToken->Name);
				break;
			}
			case LngTokenType::Operator:
			{
				const GrammarToken* const grammarToken = GrammarGetToken(token->Value.Operator);
				sprintf(buffer, "Operator:\n%5zd:\t\t\"%s\"\n\n", st, grammarToken->Name);
				break;
			}
			default:
			{
				sprintf(buffer, "Не известный тип лексемы: intValue = \"%d\".", (int)token->Type);
				break;
			}
		}

		fputs(buffer, file);
	}

	LogEndDump(LogSignature::General);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static LexerError TryReadIdentifier(const char** const text, Lexer* const lexer)
{
	assert(text);
	assert(*text);
	assert(lexer);

	const char* _text = *text;
	char c = *_text;

	// Проверяем, что начало лексемы является идентификатором.
	if (!(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_'))
		return LexerError::NotIdentifier;

	// Считываем идентификатор или ключевое слово.
	const char* identStart = _text;
	do
	{
		c = *(++_text);
	}
	while (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_');

	const size_t identLen = _text - identStart;

	KeywordType keyword = CheckKeyword(identStart, identLen);
	Token token = {};

	if (keyword == KeywordType::Null)
	{
		// Идентификатор не является ключевым словом => добавляем его в таблицу идентификаторов.
		size_t id = 0;
		IdentifierTableAddElem(lexer->Identifiers, identStart, identLen, &id);

		token.Type = LngTokenType::Identifier;
		token.Value.Identifier = id;
	}
	else
	{
		// Идентификатор является ключевым словом.
		token.Type = LngTokenType::Keyword;
		token.Value.Keyword = keyword;
	}

	ProgramStatus status = ExtArrayAddElem(&lexer->Tokens, &token);
	
	assert(status != ProgramStatus::NotImplemented);
	if (status == ProgramStatus::Fault)
	{
		TRACE_ERROR();
		return LexerError::AddToken;
	}

	*text = _text;
	return LexerError::IdentifierRead;
}

static KeywordType CheckKeyword(const char* const word, const size_t wordLength)
{
	assert(word);

	for (size_t st = 0; st < KeywordsSize; st++)
	{
		const GrammarToken keyword = Keywords[st];

		if (wordLength == keyword.NameSize && strncmp(word, keyword.Name, wordLength) == 0)
		{
			return keyword.Value.Keyword;
		}
	}

	return KeywordType::Null;
}

static LexerError TryReadOperator(const char** const text, ExtArray* const tokens)
{
	assert(text);
	assert(*text);
	assert(tokens);

	const char* const _text = *text;

	for (size_t st = 0; st < OperatorsSize; st++)
	{
		const GrammarToken oper = Operators[st];

		if (strncmp(_text, oper.Name, oper.NameSize) == 0)
		{
			Token token =
			{
				.Type  = LngTokenType::Operator,
				.Value = { .Operator = oper.Value.Operator }
			};

			ProgramStatus status = ExtArrayAddElem(tokens, &token);

			assert(status != ProgramStatus::NotImplemented);
			if (status == ProgramStatus::Fault)
			{
				TRACE_ERROR();
				return LexerError::AddToken;
			}

			*text += oper.NameSize;
			return LexerError::OperatorRead;
		}
	}

	return LexerError::NotOperator;
}

static LexerError TryReadSpecSymbol(const char** const text, ExtArray* const tokens)
{
	assert(text);
	assert(*text);
	assert(tokens);

	const char* const _text = *text;

	for (size_t st = 0; st < SpecialSymbolsSize; st++)
	{
		const GrammarToken specSym = SpecialSymbols[st];

		if (strncmp(_text, specSym.Name, specSym.NameSize) == 0)
		{
			Token token =
			{
				.Type  = LngTokenType::SpecialSymbol,
				.Value = { .SpecialSymbol = specSym.Value.SpecialSymbol }
			};

			ProgramStatus status = ExtArrayAddElem(tokens, &token);

			assert(status != ProgramStatus::NotImplemented);
			if (status == ProgramStatus::Fault)
			{
				TRACE_ERROR();
				return LexerError::AddToken;
			}

			*text += specSym.NameSize;
			return LexerError::SpecSymbolRead;
		}
	}

	return LexerError::NotSpecSymbol;
}

static LexerError TryReadNumber(const char** const text, ExtArray* const tokens)
{
	assert(text);
	assert(*text);
	assert(tokens);

	const char* _text = *text;

	// Проверяем, является ли начало лексемы числом.
	// Возможно это не число, а оператор (минус, плюс). Пытаемся считать, чтобы узнать конкретно.
	if (!(('0' <= *_text && *_text <= '9') || *_text == '-' || *_text == '+'))
		return LexerError::NotNumber;

	double number = 0;
	int read = 0;
	int scaned = sscanf(_text, "%lf%n", &number, &read);
	if (scaned != 1)
	{
		// Не является числом.
		return LexerError::NotNumber;
	}

	// Получилось считать число.
	Token token =
	{
		.Type  = LngTokenType::Number,
		.Value = { .Number = number }
	};

	ProgramStatus status = ExtArrayAddElem(tokens, &token);

	assert(status != ProgramStatus::NotImplemented);
	if (status == ProgramStatus::Fault)
	{
		TRACE_ERROR();
		return LexerError::AddToken;
	}

	*text = _text + read;
	return LexerError::NumberRead;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static void SkipSpaceSymbols(const char** const text)
{
	assert(text);
	assert(*text);

	const char* _text = *text;
	while (*_text && (*_text == ' ' || *_text == '\t' || *_text == '\n'))
		_text++;
	*text = _text;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///