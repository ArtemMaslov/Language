#include <assert.h>
#include <stdio.h>

#include "../../Front end/Parser.h"
#include "../../AST/AST.h"

#include "x86Compiler.h"
#include "x86CompilerUnitTests.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define CHECK_STATUS \
	assert (status == ProgramStatus::Ok)

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static const char* testFiles[] =
{
	//"../../Tests/simple ret.lng",
	//"../../Tests/simple hello world.lng",
	//"../../Tests/quadratic equation 1.lng",
	//"../../Tests/factorial recursion.lng",
	//"../../Tests/factorial while.lng",
	"../../Tests/circle_x86.lng",
	//"../../Tests/call funct.lng",
};

static const size_t testFilesCount = sizeof(testFiles) / sizeof(char*);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

void x86CompilerTest1()
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

		char listing[256] = "";

		x86Compiler comp = {};

		status = x86CompilerConstructor(&comp, &ast);

		status = x86CompileAsmRep(&comp);
		CHECK_STATUS;

		AsmListingType type = {};
		type.Commands = 1;
		type.Codes    = 0;
		sprintf(listing, "comp%zd_cmd.lst", st);
		status = x86WriteListingFile(&comp, listing, type);
		CHECK_STATUS;

		status = AsmTranslateCommands(&comp);
		CHECK_STATUS;

		type.Codes    = 1;

		status = x86CompileJitBuffer(&comp);
		CHECK_STATUS;

		sprintf(listing, "comp%zd_code.lst", st);
		status = x86WriteListingFile(&comp, listing, type);
		CHECK_STATUS;

		status = x86DumpJIT(&comp, "jitDump.txt");
		CHECK_STATUS;

		status = x86RunJIT(&comp);
		CHECK_STATUS;

		x86CompilerDestructor(&comp);

		status = AstDestructor(&ast);
		CHECK_STATUS;

		status = ParserDestructor(&parser);
	}

	CHECK_STATUS;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///