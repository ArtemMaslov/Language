#include <stdio.h>
#include <Windows.h>

#include "Modules/Logs/Logs.h"
#include "Front end/Lexer/LexerUnitTests.h"
#include "Front end/ParserUnitTests.h"
#include "Back end/x86/x86CompilerUnitTests.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

int main(int argc, char* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	if (LogsConstructor() != LogError::NoErrors)
	{
		puts("������ �������� ����� �����.");
		return 1;
	}

	//LexerTest1();
	//ParserTest1();
	x86CompilerTest1();

	LogsDestructor();
	return 0;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///