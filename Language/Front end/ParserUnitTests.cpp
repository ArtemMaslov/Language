#include <assert.h>
#include <stdio.h>

#include "ParserUnitTests.h"
#include "Parser.h"
#include "../AST/AST.h"
#include "../AST/AST_Verify.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define CHECK_STATUS \
	assert (status == ProgramStatus::Ok)

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static const char* testFiles[] =
{
	//"../../Tests/simple hello world.lng",
	//"../../Tests/quadratic equation 1.lng",
	//"../../Tests/factorial recursion.lng",
	//"../../Tests/factorial while.lng",
	"../../Tests/circle.lng",
};

static const size_t testFilesCount = sizeof(testFiles) / sizeof(char*);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

void ParserTest1()
{
	Parser parser = {};

	ProgramStatus status = ProgramStatus::Ok;

	for (size_t st = 0; st < testFilesCount; st++)
	{
		AST ast = {};

		status = ParserConstructor(&parser);
		CHECK_STATUS;
		
		status = AstConstructor(&ast);
		CHECK_STATUS;

		status = ParserParseFile(&parser, &ast, testFiles[st]);
		CHECK_STATUS;

		status = AstGraphicDump(&ast);
		//CHECK_STATUS;

		status = AstDestructor(&ast);
		CHECK_STATUS;

		status = ParserDestructor(&parser);
		CHECK_STATUS;
	}
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 