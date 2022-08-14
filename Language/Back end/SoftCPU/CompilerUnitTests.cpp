#include <assert.h>
#include <stdio.h>

#include "../../Front end/Parser.h"
#include "../../AST/AST.h"
#include "Compiler.h"
#include "CompilerUnitTests.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define CHECK_STATUS \
	assert (status == ProgramStatus::Ok)

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static const char* testFiles[] =
{
	//"../../Tests/simple hello world.lng",
	"../../Tests/quadratic equation 1.lng",
	"../../Tests/factorial recursion.lng",
	"../../Tests/factorial while.lng",
};

static const size_t testFilesCount = sizeof(testFiles) / sizeof(char*);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

void CompilerTest1()
{
	ProgramStatus status = ProgramStatus::Ok;

	for (size_t st = 0; st < testFilesCount; st++)
	{
		Parser parser = {};
		AST ast = {};

		status = ParserConstructor(&parser);
		CHECK_STATUS;

		status = AstConstructor(&ast);
		CHECK_STATUS;

		status = ParserParseFile(&parser, &ast, testFiles[st]);
		CHECK_STATUS;

		char buffer[256] = "";
		sprintf(buffer, "compiled%zd.code", st);

		status = CompileFile(&ast, buffer);
		CHECK_STATUS;

		status = AstDestructor(&ast);
		CHECK_STATUS;

		status = ParserDestructor(&parser);
	}

	CHECK_STATUS;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 