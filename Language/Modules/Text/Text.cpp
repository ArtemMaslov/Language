#include <assert.h>
#include <stdlib.h>

#include "../ErrorsHandling.h"
#include "../FilesIO/FilesIO.h"

#include "Text.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus TextConstructor(Text* text)
{
	assert(text);
	
	assert(text->Data   == nullptr);
	assert(text->Size   == 0);
	assert(text->Readed == false);

	return ProgramStatus::Ok;
}

ProgramStatus TextDestructor(Text* text)
{
	assert(text);

	free(text->Data);
	
	text->Data   = nullptr;
	text->Size   = 0;
	text->Readed = false;

	return ProgramStatus::Ok;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus TextReadFile(Text* text, const char* fileName)
{
	assert(text);
	assert(fileName);

	FILE* file = fopen(fileName, "r");

	if (!file)
	{
		return ProgramStatus::Fault;
	}

	const size_t fileSize   = GetFileSize(file);
	const size_t bufferSize = fileSize + 1;

	char* buffer = (char*)calloc(bufferSize, sizeof(char));

	if (!buffer)
	{
		return ProgramStatus::Fault;
	}

	size_t readed = fread(buffer, sizeof(char), fileSize, file);

	fclose(file);

	text->Data   = buffer;
	text->Size   = bufferSize;
	text->Readed = true;

	return ProgramStatus::Ok;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 