#include <assert.h>
#include <stdio.h>

#include "LexerUnitTests.h"
#include "Lexer.h"
#include "../../Modules/ErrorsHandling.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define CHECK_STATUS \
	assert (status == ProgramStatus::Ok)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static const char* testFiles[] =
{
	//"../../Tests/simple hello world.lng",
	"../../Tests/quadratic equation 1.lng",
	"../../Tests/factorial recursion.lng",
	"../../Tests/factorial while.lng",
};

static const size_t testFilesCount = sizeof(testFiles) / sizeof(char*);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

void LexerTest1()
{
	Lexer lexer = {};

	ProgramStatus status = ProgramStatus::Ok;

	for (size_t st = 0; st < testFilesCount; st++)
	{
		status = LexerConstructor(&lexer);
		CHECK_STATUS;

		status = LexerReadFile(&lexer, testFiles[st]);
		CHECK_STATUS;

		status = LexerGetTokens(&lexer);
		CHECK_STATUS;

		status = LexerLogDump(&lexer);
		CHECK_STATUS;

		status = LexerDestructor(&lexer);
		CHECK_STATUS;
	}
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///