///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Тестирование модуля компилятора x86.
// 
// Использование:
// 1. Настроить файл конфигурации модульного тестирования "project/UnitTest.h":
//     1.1. Установить ENABLE_UNIT_TESTS true.
//     1.2. Установить ENABLE_X86_COMPILER_TEST_RUN 1 или ENABLE_X86_COMPILER_TEST_COMPILE 1.
// 
// 2. Скомпилировать и запустить программу.
// 
// Версия: 1.0.1.1
// Дата последнего изменения: 14:53 13.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include "../../UnitTests.h"

#if ENABLE_UNIT_TESTS

#include <assert.h>
#include <stdio.h>

#include "../../Modules/Logs/Logs.h"

#include "../../Modules/TargetOS.h"
#if defined(WINDOWS)
#include <Windows.h>
#endif

#include "../../Front end/Parser.h"
#include "../../AST/AST.h"

#include "x86Compiler.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Тестовые программы.
static const char* testFiles[] =
{
	//"../../Tests/simple ret.lng",
	//"../../Tests/simple hello world.lng",
	//"../../Tests/quadratic equation 1.lng",
	//"../../Tests/quadratic equation 2.lng",
	//"../../Tests/factorial recursion.lng",
	//"../../Tests/factorial while.lng",
	"../../Tests/circle_x86.lng",
	//"../../Tests/call funct.lng",
};

/// Количество тестовых программ.
static const size_t testFilesCount = sizeof(testFiles) / sizeof(char*);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#if ENABLE_X86_COMPILER_TEST_RUN

int main()
{
#if defined(WINDOWS)
SetConsoleCP(65001);
SetConsoleOutputCP(65001);
#endif

	LogError logErr = LogsConstructor();
	if (logErr != LogError::NoErrors)
	{
		puts("Ошибка открытия файла логов.");
		return (int)logErr;
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
		LexerDestructor(&lexer);
		
		if (AstGraphicDump(&ast) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		x86Compiler comp = {};

		if (x86CompilerConstructor(&comp, &ast) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}
		
		if (x86CompileToAsm(&comp) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		char listing[32] = "";
		AsmListingType type = {};
		type.Commands = 1;
		type.Codes    = 0;

		sprintf(listing, "comp%zd_cmd.lst", st);
		if (x86WriteListingFile(&comp, listing, type) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		if (x86TranslateToByteCode(&comp) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		if (x86CompileJitBuffer(&comp) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		sprintf(listing, "comp%zd_code.lst", st);
		type.Codes = 1;

		if (x86WriteListingFile(&comp, listing, type) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		if (x86DumpJIT(&comp, "jitDump.txt") != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		if (x86RunJIT(&comp) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		x86CompilerDestructor(&comp);
		AstDestructor(&ast);
		IdentifierTableDestructor(&table);
	}

	LogsDestructor();
	return 0;
}

#endif // !ENABLE_X86_COMPILER_TEST_RUN

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#if ENABLE_X86_COMPILER_TEST_COMPILE

int main()
{
#if defined(WINDOWS)
SetConsoleCP(65001);
SetConsoleOutputCP(65001);
#endif

	LogError logError = LogsConstructor();
	if (logError != LogError::NoErrors)
	{
		puts("Ошибка открытия файла логов.");
		return (int)logError;
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

		LexerLogDump(&lexer);

		if (ParserParseFile(&lexer, &ast) != ParserError::NoErrors)
		{
			TRACE_ERROR();
			return -1;
		}
		LexerDestructor(&lexer);

		if (AstGraphicDump(&ast) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return -1;
		}

		AstDestructor(&ast);
		IdentifierTableDestructor(&table);
	}

	LogsDestructor();

	return 0;
}

#endif // !ENABLE_X86_COMPILER_TEST_COMPILE

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !ENABLE_UNIT_TESTS