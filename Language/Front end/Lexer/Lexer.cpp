#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Lexer.h"
#include "../../Modules/ErrorsHandling.h"
#include "../../Modules/Logs/Logs.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define CHECK_STATUS \
	if (status != ProgramStatus::Ok) \
		return status

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static KeywordType CheckKeyword(const char* name, const size_t nameLength);

static OperatorType CheckOperator(const char** text);

static SpecialSymbolType CheckSpecSymbol(const char** text);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus LexerConstructor(Lexer* lexer)
{
	assert(lexer);

	ProgramStatus status = ProgramStatus::Ok;

	status = TextConstructor(&lexer->Text);
	CHECK_STATUS;
	status = ExtArrayConstructor(&lexer->Commands, sizeof(Token));
	CHECK_STATUS;
	status = IdentifierTableConstructor(&lexer->IdentifierTable);

	return status;
}

ProgramStatus LexerDestructor(Lexer* lexer)
{
	assert(lexer);

	ProgramStatus status = ProgramStatus::Ok;

	status = TextDestructor(&lexer->Text);
	CHECK_STATUS;
	status = ExtArrayDestructor(&lexer->Commands);
	CHECK_STATUS;
	status = IdentifierTableDestructor(&lexer->IdentifierTable);

	return status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus LexerGetTokens(Lexer* lexer)
{
	assert(lexer);
	assert(&lexer->Text.Readed);

	ProgramStatus status = ProgramStatus::Ok;

	const char*  text     = lexer->Text.Data;
	const size_t textSize = lexer->Text.Size;

	ExtArray* const tokens = &lexer->Commands;

	while (*text)
	{
		char c = *text;

		if (c == ' ' || c == '\t' || c == '\n')
		{
			text++;
			continue;
		}
		else if ('0' <= c && c <= '9')
		{
			// Считываем число.
			double number = 0;
			size_t readed = 0;
			int scaned = sscanf(text, "%lf%zn", &number, &readed);

			if (scaned != 1)
			{
				assert(!"Error");
				return ProgramStatus::Fault;
			}

			Token token =
			{
				.Type  = LngTokenType::Number,
				.Value = { .Number = number }
			};

			ExtArrayAddElem(tokens, &token);

			text += readed;
			continue;
		}
		else if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_')
		{
			const char* word = text;
			// Считываем идентификатор или ключевое слово.
			do
			{
				c = *(++text);
			}
			while ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9' || c == '_');

			const size_t wordLen = text - word;

			KeywordType keyword = CheckKeyword(word, wordLen);

			if (keyword == KeywordType::Null)
			{
				int id = 0;
				IdentifierTableAddElem(&lexer->IdentifierTable, word, wordLen, &id);

				Token token =
				{
					.Type  = LngTokenType::Identifier,
					.Value = { .Identifier = id }
				};

				ExtArrayAddElem(tokens, &token);
			}
			else
			{
				Token token =
				{
					.Type  = LngTokenType::Keyword,
					.Value = { .Keyword = keyword }
				};

				ExtArrayAddElem(tokens, &token);
			}

			continue; // Обрабатываем символ за идентификатором ещё раз.
		}
		else
		{
			// Считываем специальные символы ('(', '[', ...) и символьные операторы ('+', '-', '==', ...)
			OperatorType oper = CheckOperator(&text);

			if (oper != OperatorType::Null)
			{
				Token token =
				{
					.Type  = LngTokenType::Operator,
					.Value = { .Operator = oper }
				};

				ExtArrayAddElem(tokens, &token);
				continue;
			}

			SpecialSymbolType specSym = CheckSpecSymbol(&text);

			if (specSym != SpecialSymbolType::Null)
			{
				Token token =
				{
					.Type  = LngTokenType::SpecialSymbol,
					.Value = { .SpecialSymbol = specSym }
				};

				ExtArrayAddElem(tokens, &token);
				continue;
			}

			// Не удалось обработать символ. Выдаём ошибку.
			assert(!"Error");
			return ProgramStatus::Fault;
		}
	}

	return status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus LexerReadFile(Lexer* lexer, const char* fileName)
{
	assert(lexer);

	ProgramStatus status = ProgramStatus::Ok;

	status = TextReadFile(&lexer->Text, fileName);

	return status;
}

ProgramStatus LexerLogDump(Lexer* lexer)
{
	FILE* file = LogBeginDump(LOG_SIG_GENERAL, LOG_LVL_DEBUG);

	char buffer[1024] = "";

	fputs("Срез состояния лексера.\n", file);

	const size_t tokensCount = lexer->Commands.Size;
	for (size_t st = 0; st < tokensCount; st++)
	{
		const Token* token = (Token*)ExtArrayGetElemAt(&lexer->Commands, st);

		switch (token->Type)
		{
			case LngTokenType::Keyword:
			{
				const GrammarToken* grammarToken = GrammarGetName(token->Type, (int)token->Value.Keyword);
				sprintf(buffer, "Keyword:\n%5zd:\t\t\"%s\"\n\n", st, grammarToken->Name);
				break;
			}
			case LngTokenType::Identifier:
			{
				const Identifier* id = IdentifierGetById(&lexer->IdentifierTable, (int)token->Value.Identifier);
				assert(id);
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
				const GrammarToken* grammarToken = GrammarGetName(token->Type, (int)token->Value.SpecialSymbol);
				sprintf(buffer, "SpecSym:\n%5zd:\t\t\"%s\"\n\n", st, grammarToken->Name);
				break;
			}
			case LngTokenType::Operator:
			{
				const GrammarToken* grammarToken = GrammarGetName(token->Type, (int)token->Value.Operator);
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

	LogEndDump(LOG_SIG_GENERAL);

	return ProgramStatus::Ok;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static KeywordType CheckKeyword(const char* word, const size_t wordLength)
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

static OperatorType CheckOperator(const char** text)
{
	assert(text);
	assert(*text);

	const char* _text = *text;

	for (size_t st = 0; st < OperatorsSize; st++)
	{
		const GrammarToken oper = Operators[st];

		if (strncmp(_text, oper.Name, oper.NameSize) == 0)
		{
			*text += oper.NameSize;
			return oper.Value.Operator;
		}
	}
	
	return OperatorType::Null;
}

static SpecialSymbolType CheckSpecSymbol(const char** text)
{
	assert(text);
	assert(*text);

	const char* _text = *text;

	for (size_t st = 0; st < SpecialSymbolsSize; st++)
	{
		const GrammarToken specSym = SpecialSymbols[st];

		if (strncmp(_text, specSym.Name, specSym.NameSize) == 0)
		{
			*text += specSym.NameSize;
			return specSym.Value.SpecialSymbol;
		}
	}

	return SpecialSymbolType::Null;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#undef CHECK_STATUS