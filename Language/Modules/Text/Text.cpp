#include <assert.h>
#include <stdlib.h>

#include "../ErrorsHandling.h"
#include "../FilesIO/FilesIO.h"

#include "Text.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

int TextConstructor(Text* text)
{
	assert(text);
	
	assert(text->Data == nullptr);
	assert(text->Size == 0);

	return PROGRAM_NO_ERRORS;
}

void TextDestructor(Text* text)
{
	assert(text);

	free(text->Data);
	
	text->Data = nullptr;
	text->Size = 0;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

int TextReadFile(Text* text, const char* fileName)
{
	assert(text);
	assert(fileName);

	FILE* file = fopen(fileName, "r");

	if (!file)
	{
		return PROGRAM_FAULT;
	}

	const size_t fileSize = GetFileSize(file);

	char* buffer = (char*)calloc(fileSize + 1, sizeof(char));

	if (!buffer)
	{
		return PROGRAM_FAULT;
	}

	size_t readed = fread(buffer, sizeof(char), fileSize, file);

	fclose(file);

	return PROGRAM_NO_ERRORS;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\