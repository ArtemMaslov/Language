///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Тестирование модуля компилятора для эмулятора процессора SoftCPU.
// 
// Использование:
// 1. Настроить файл конфигурации модульного тестирования "project/UnitTest.h":
//     1.1. Установить ENABLE_UNIT_TESTS true.
//     1.2. Установить ENABLE_SOFT_CPU_TEST_RUN 1.
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
#if ENABLE_SOFT_CPU_TEST_RUN

#include <assert.h>
#include <stdio.h>

#include "../../Modules/TargetOS.h"
#if defined(WINDOWS)
#include <Windows.h>
#endif

#include "../../Modules/Logs/Logs.h"

#include "../../Front end/Parser.h"
#include "../../AST/AST.h"
#include "SoftCpuCompiler.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Тестовые программы.
static const char* testFiles[] =
{
	"../../Tests/simple hello world.lng",
	"../../Tests/quadratic equation 1.lng",
	"../../Tests/factorial recursion.lng",
	"../../Tests/factorial while.lng",
	"../../Tests/circle_soft_cpu.lng",
};

/// Количество тестовых программ.
static const size_t testFilesCount = sizeof(testFiles) / sizeof(char*);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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

		char buffer[256] = "";
		sprintf(buffer, "compiled%zd.code", st);

		if (SoftCpuCompileFile(&ast, buffer) != ProgramStatus::Ok)
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

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !ENABLE_SOFT_CPU_TEST_RUN
#endif // !ENABLE_UNIT_TESTS