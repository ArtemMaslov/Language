#include "UnitTests.h"
#if not ENABLE_UNIT_TESTS

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "Modules/Logs/Logs.h"

#include "Modules/TargetOS.h"
#if defined(WINDOWS)
#include <Windows.h>
#endif

#include "Front end/Parser.h"
#include "AST/AST.h"

#include "Back end/x86/x86Compiler.h"
#include "Back end/SoftCPU/SoftCpuCompiler.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Тип архитектуры компилятора.
enum class ArchType
{
	/// Эмулятор процессора SoftCpu.
	SoftCpu,
	/// Архитектура Intel x86 (32-разрядная).
	x86
};

/// Структура аргументов компилятора.
struct CommandLineArgs
{
	/// Имя входного файла программы на языке Language.
	char*          InputFile;
	/// Имя выходного файла.
	/// Для архитектуры SoftCpu - имя выходного файла ассемблера SoftCpu.
	/// Для всех архитектур: Имя без расширения используется для формирования различных
	/// листингов и дампов.
	char*          OutputFile;
	/// Длина расширения имени выходного файла общая.
	size_t         OutputFileFormatSize;
	/// Длина имени выходного файла без расширения.
	size_t         OutputFileNameSize;

	/// Архитектура, под которую компилируется программа.
	ArchType       Arch;

	/// Если true, то будет выведен дамп лексера в файл логов.
	bool           LexerDump;
	/// Если true, то будет создано графический рисунок АСД с помощью GraphViz.
	bool           AstGraphDump;

	/// Если true, то будет создан файл листинга компиляция под x86.
	bool           x86Listing;
	/// Тип листинга компиляции под x86.
    AsmListingType x86ListingType;
	/// Если true, то будет создан дамп-файл Jit-буфера x86.
	bool           JitBufferDump;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Проанализировать полное имя файла. Определить длину имени файла и длину расширения.
 * "Полное имя файла" = "имя файла" + "." + "расширение".
 * 
 * @param fileName      Полное имя файла.
 * @param outNameSize   Выходная длина имени файла.
 * @param outFormatSize Выходная длина расширения.
 */
static void ParseFileFormat(const char* const fileName, size_t* const outNameSize, size_t* const outFormatSize);

/**
 * @brief  Проанализировать аргументы командной строки.
 * 
 * @param argc    Количество аргументов командной строки.
 * @param argv    Аргументы командной строки.
 * @param cmdArgs Выходная структура. Проанализированные аргументы.
 * 
 * @return false, если в командной строке есть ошибка.
 */
static bool ParseCommandLine(const int argc, char* argv[], CommandLineArgs* const cmdArgs);

/**
 * @brief  Запустить лексический и синтаксический анализаторы.
 * 
 * @param ast   Выходное абстрактное синтаксическое дерево.
 * @param table Выходная таблица идентификаторов.
 * @param args  Аргументы командной строки.
 * 
 * @return false, если произошла ошибка при выполнении функции.
 */
static bool RunFrontend(AST* const ast, IdentifierTable* const table, const CommandLineArgs* const args);

/**
 * @brief  Запустить компилятор под архитектуру Intel x86.
 * 
 * @param ast  Указатель на АСД.
 * @param args Аргументы командной строки.
 * 
 * @return false, если произошла ошибка при выполнении функции.
 */
static bool RunCompilerX86(AST* const ast, const CommandLineArgs* const args);

/**
 * @brief  Запустить компилятор под архитектуру SoftCpu.
 * 
 * @param ast  Указатель на АСД.
 * @param args Аргументы командной строки.
 * 
 * @return false, если произошла ошибка при выполнении функции.
 */
static bool RunCompilerSoftCpu(AST* const ast, const CommandLineArgs* const args);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

int main(int argc, char* argv[])
{
#if defined(WINDOWS)
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
#endif

	LogError logErr = LogsConstructor();
	if (logErr != LogError::NoErrors)
	{
		puts("Ошибка открытия файла логов.");
		return (int)logErr;
	}

	CommandLineArgs args = {};
	if (!ParseCommandLine(argc, argv, &args))
	{
		TRACE_ERROR();
		LogsDestructor();
		return -1;
	}

	AST ast = {};
	IdentifierTable table = {};
	
	if (!RunFrontend(&ast, &table, &args))
	{
		TRACE_ERROR();
		AstDestructor(&ast);
		IdentifierTableDestructor(&table);
		LogsDestructor();
		return -1;
	}

	switch (args.Arch)
	{
	case ArchType::x86:
		if (RunCompilerX86(&ast, &args))
		{
			TRACE_ERROR();
			AstDestructor(&ast);
			IdentifierTableDestructor(&table);
			LogsDestructor();
			return -1;
		}
		break;

	case ArchType::SoftCpu:
		if (RunCompilerSoftCpu(&ast, &args))
		{
			TRACE_ERROR();
			AstDestructor(&ast);
			IdentifierTableDestructor(&table);
			LogsDestructor();
			return -1;
		}
		break;
	
	default:
		// Заглушка, чтобы не забыть дописать здесь код при добавлении новой архитектуры.
		assert(!"Not implemented");
		return -2;
	}

	AstDestructor(&ast);
	IdentifierTableDestructor(&table);

	LogsDestructor();
	return 0;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define COMPARE_ARG(arg) \
	(strncmp(argv[argIndex], (arg), sizeof(arg) - 1) == 0)

static void ParseFileFormat(const char* const fileName, size_t* const outNameSize, size_t* const outFormatSize)
{
	assert(fileName);
	//assert(outNameSize);
	//assert(outFormatSize);

	size_t outFileFullNameSize = strlen(fileName);
	for (int st1 = outFileFullNameSize - 1; st1 >= 0; st1--)
	{
		if (fileName[st1] == '.')
		{
			if (outFormatSize)
			{
				*outFormatSize = outFileFullNameSize - st1 - 1;
			}

			if (outNameSize)
			{
				if (st1 > 0)
					*outNameSize = st1 - 1;
				else
					*outNameSize = 0;
			}
			return;
		}
	}
	
	// '.' не найдена в имени файла => файл без расширения.
	if (outFormatSize)
		*outFormatSize = 0;
	if (outNameSize)
		*outNameSize = outFileFullNameSize;
}

static bool ParseCommandLine(const int argc, char* argv[], CommandLineArgs* const cmdArgs)
{
	assert(argv);
	assert(cmdArgs);

	size_t outFileNameSize       = 0;
	size_t outFileNameFormatSize = 0;

	bool inputFile  = false;
	bool outputFile = false;
	cmdArgs->Arch = ArchType::x86;

	for (int argIndex = 1; argIndex < argc; argIndex++)
	{
		if (COMPARE_ARG("-o"))
		{
			if (outputFile)
			{
				puts("Введите только один выходной файл.");
				return false;
			}

			argIndex++;
			if (argIndex >= argc)
			{
				puts("После параметра -o не указано имя выходного файла.");
				return false;
			}

			cmdArgs->OutputFile = argv[argIndex];

			ParseFileFormat(cmdArgs->OutputFile, &cmdArgs->OutputFileNameSize, &cmdArgs->OutputFileFormatSize);
			outputFile = true;
		}
		else if (COMPARE_ARG("-iscpu"))
		{
			cmdArgs->Arch = ArchType::SoftCpu;
		}
		else if (COMPARE_ARG("-ix86"))
		{
			cmdArgs->Arch = ArchType::x86;
		}
		else if (COMPARE_ARG("-dump_lex"))
		{
			cmdArgs->LexerDump = true;
		}
		else if (COMPARE_ARG("-dump_ast"))
		{
			cmdArgs->AstGraphDump = true;
		}
		else if (COMPARE_ARG("-list_x86_code"))
		{
			cmdArgs->x86Listing = true;
			cmdArgs->x86ListingType.Codes = 1;
		}
		else if (COMPARE_ARG("-list_x86_cmds"))
		{
			cmdArgs->x86Listing = true;
			cmdArgs->x86ListingType.Commands = 1;
		}
		else if (COMPARE_ARG("-dump_jit"))
		{
			cmdArgs->JitBufferDump = true;
		}
		else
		{
			if (inputFile)
			{
				puts("Компилятор может за раз обрабатывать только один входной файл.");
				return false;
			}

			size_t formatSize = 0;
			size_t nameSize   = 0;
			ParseFileFormat(argv[argIndex], &nameSize, &formatSize);

			size_t lngFormatSize = sizeof(".lng") - 1 - 1; // Учитываем '.', '\0'
			if (formatSize != lngFormatSize ||
				strncmp(argv[argIndex] + nameSize + 1, ".lng", lngFormatSize) != 0)
			{
				printf("Не распознанный аргумент \"%s\".\n"
				       "Ошибка интерпретации как имени входного файла.\n"
					   "Расширение входного файла должно быть \".lng\".\n", argv[argIndex]);
				return false;
			}

			cmdArgs->InputFile = argv[argIndex];
			inputFile = true;
		}
	}

	if (!outputFile)
	{
		cmdArgs->OutputFile = (char*)"compiled.code";
		ParseFileFormat(cmdArgs->OutputFile, &cmdArgs->OutputFileNameSize, &cmdArgs->OutputFileFormatSize);
	}

	if (!inputFile)
	{
		puts("Введите имя входного файла.");
		return false;
	}

	return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static bool RunFrontend(AST* const ast, IdentifierTable* const table, const CommandLineArgs* const args)
{
	assert(ast);
	assert(table);
	assert(args);

	if (AstConstructor(ast) != AstError::NoErrors)
	{
		TRACE_ERROR();
		return false;
	}

	if (IdentifierTableConstructor(table) != IdentifierError::NoErrors)
	{
		TRACE_ERROR();
		return false;
	}

	Lexer lexer = {};
	if (LexerConstructor(&lexer, table) != LexerError::NoErrors)
	{
		TRACE_ERROR();
		return false;
	}

	if (LexerAnalyseFile(&lexer, args->InputFile) != LexerError::NoErrors)
	{
		TRACE_ERROR();
		LexerDestructor(&lexer);
		return false;
	}

	if (ParserParseFile(&lexer, ast) != ParserError::NoErrors)
	{
		TRACE_ERROR();
		LexerDestructor(&lexer);
		return false;
	}

	if (args->LexerDump)
		LexerLogDump(&lexer);

	LexerDestructor(&lexer);

	if (args->AstGraphDump)
	{
		if (AstGraphicDump(ast) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			return false;
		}
	}

	return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static bool RunCompilerX86(AST* const ast, const CommandLineArgs* const args)
{
	assert(ast);
	assert(args);

	x86Compiler comp = {};
	if (x86CompilerConstructor(&comp, ast) != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return false;
	}
	
	if (x86CompileToAsm(&comp) != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		x86CompilerDestructor(&comp);
		return false;
	}

	if (x86TranslateToByteCode(&comp) != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		x86CompilerDestructor(&comp);
		return false;
	}
	
	if (args->x86Listing)
	{
		const size_t bufferSize = 256;
		char listing[bufferSize] = "";

		snprintf(listing, bufferSize, "%*s.lst", (int)args->OutputFileNameSize, args->OutputFile);
		if (x86WriteListingFile(&comp, listing, args->x86ListingType) != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			x86CompilerDestructor(&comp);
			return false;
		}
	}

	if (x86CompileJitBuffer(&comp) != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		x86CompilerDestructor(&comp);
		return false;
	}

	if (args->JitBufferDump)
	{
		if (x86DumpJIT(&comp, "jitDump.txt") != ProgramStatus::Ok)
		{
			TRACE_ERROR();
			x86CompilerDestructor(&comp);
			return false;
		}
	}

	if (x86RunJIT(&comp) != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		x86CompilerDestructor(&comp);
		return false;
	}

	x86CompilerDestructor(&comp);
	return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static bool RunCompilerSoftCpu(AST* const ast, const CommandLineArgs* const args)
{
	assert(ast);
	assert(args);

	if (SoftCpuCompileFile(ast, args->OutputFile) != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return false;
	}

	return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif //!(not ENABLE_UNIT_TESTS)