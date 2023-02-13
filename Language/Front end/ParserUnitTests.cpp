///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Тестирование модуля синтаксического анализатора.
// 
// Использование:
// 1. Настроить файл конфигурации модульного тестирования "project/UnitTest.h":
//     1.1. Установить ENABLE_UNIT_TESTS true.
//     1.2. Установить ENABLE_PARSER_TEST 1.
// 
// 2. Скомпилировать и запустить программу.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 13:08 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include "../UnitTests.h"

#if ENABLE_UNIT_TESTS
#if ENABLE_PARSER_TEST

#include <assert.h>
#include <stdio.h>

#include "../Modules/TargetOS.h"

#if defined(WINDOWS)
#include <Windows.h>
#endif

#include "../Modules/Logs/Logs.h"
#include "../Modules/ErrorsHandling.h"

#include "../AST/AST.h"
#include "Parser.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Набор тестовых программ.
static const char* testFiles[] =
{
	//"../../Tests/simple hello world.lng",
	//"../../Tests/quadratic equation 1.lng",
	//"../../Tests/factorial recursion.lng",
	//"../../Tests/factorial while.lng",
	"../../Tests/circle_x86.lng",
};

/// Количество тестовых программ.
static const size_t testFilesCount = sizeof(testFiles) / sizeof(char*);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

int main()
{
#if defined(WINDOWS)
SetConsoleCP(1251);
SetConsoleOutputCP(1251);
#endif

	LogError status = LogsConstructor();
	if (status != LogError::NoErrors)
	{
		puts("Ошибка открытия файла логов.");
		return (int)status;
	}
	
	for (size_t st = 0; st < testFilesCount; st++)
	{
		AST ast = {};

		if (AstConstructor(&ast) != AstError::NoErrors)
		{
			TRACE_ERROR();
			return -1;
		}

		IdentifierTable table = {};
		if (IdentifierTableConstructor(&table) != IdentifierError::NoErrors)
		{
			TRACE_ERROR();
			return -1;
		}

		Lexer lexer = {};
		if (LexerConstructor(&lexer, &table) != LexerError::NoErrors)
		{
			TRACE_ERROR();
			return -1;
		}

		if (LexerAnalyseFile(&lexer, testFiles[st]) != LexerError::NoErrors)
		{
			TRACE_ERROR();
			return -1;
		}

		if (ParserParseFile(&lexer, &ast) != ParserError::NoErrors)
		{
			TRACE_ERROR();
			return -1;
		}

		if (AstGraphicDump(&ast) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		AstDestructor(&ast);
		LexerDestructor(&lexer);
		IdentifierTableDestructor(&table);
	}

	LogsDestructor();
	return 0;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !ENABLE_PARSER_TEST
#endif // !ENABLE_UNIT_TESTS