///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Тестирование модуля лексического анализатора.
// 
// Использование:
// 1. Настроить файл конфигурации модульного тестирования "project/UnitTest.h":
//     1.1. Установить ENABLE_UNIT_TESTS true.
//     1.2. Установить ENABLE_LEXER_TEST 1.
// 
// 2. Скомпилировать и запустить программу.
// 
// Версия: 1.0.1.1
// Дата последнего изменения: 13:05 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include "../../UnitTests.h"
#if ENABLE_UNIT_TESTS
#if ENABLE_LEXER_TEST

#include <assert.h>
#include <stdio.h>

#include "../../Modules/TargetOS.h"

#if defined(WINDOWS)
#include <Windows.h>
#endif

#include "../../Modules/Logs/Logs.h"
#include "../../Modules/ErrorsHandling.h"

#include "Lexer.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Набор тестовых программ.
static const char* testFiles[] =
{
	//"../../Tests/simple hello world.lng",
	"../../Tests/quadratic equation 1.lng",
	"../../Tests/factorial recursion.lng",
	"../../Tests/factorial while.lng",
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

	LogError status = LogsConstructor();
	if (status != LogError::NoErrors)
	{
		puts("Ошибка открытия файла логов.");
		return (int)status;
	}

	for (size_t st = 0; st < testFilesCount; st++)
	{
		LOG_F_INFO("Начало лексического анализа файла \"%s\".", testFiles[st]);

		IdentifierTable table = {};

		IdentifierError identErr = IdentifierError::NoErrors;
		if ((identErr = IdentifierTableConstructor(&table)) != IdentifierError::NoErrors)
		{
			puts("Ошибка создания таблицы идентификаторов.");
			return (int)identErr;
		}

		Lexer lexer = {};

		if (LexerConstructor(&lexer, &table) != LexerError::NoErrors)
		{
			TRACE_ERROR();
			continue;
		}

		if (LexerAnalyseFile(&lexer, testFiles[st]) != LexerError::NoErrors)
		{
			TRACE_ERROR();
			LexerDestructor(&lexer);
			continue;
		}

		LexerLogDump(&lexer);

		LexerDestructor(&lexer);
		IdentifierTableDestructor(&table);
	}

	LogsDestructor();
	return 0;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !ENABLE_LEXER_TEST
#endif // !ENABLE_UNIT_TESTS