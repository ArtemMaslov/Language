///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль чтения файлов.
// 
// Файл с исходным кодом.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 17:54 28.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../FilesIO/FilesIO.h"

#include "../ErrorsHandling.h"
#include "../Logs/Logs.h"

#include "Text.h"

#if MODULE_TEXT_DISABLE_LOGS
#include "../DisableLogs.h"
#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

TextError TextConstructor(Text* const text, const char* const fileName, FileReadMode mode)
{
	assert(text);
	assert(fileName);

	char modeStr[3] = "r";
	if (mode == FileReadMode::Binary)
		modeStr[1] = 'b';

	FILE* file = fopen(fileName, modeStr);
	if (!file)
	{
		assert(!"Ошибка открытия файла");
		LOG_F_ERR("Ошибка открытия файла \"%s\".", fileName);
		return TextError::FileOpen;
	}

	const size_t fileSize   = GetFileSize(file);
	const size_t BufferSize = fileSize + 1;

	char* buffer = (char*)calloc(BufferSize, sizeof(char));
	if (!buffer)
	{
		LOG_ERR_MEMORY(BufferSize * sizeof(char));
		return TextError::Memory;
	}

	size_t read = fread(buffer, sizeof(char), fileSize, file);

	// Если файл читается как текстовый, то read <= fileSize, так как, например, '\r\n' заменяются на '\n'.
	// Если файл читается как бинарный, то read == fileSize, в случае чтения файла без ошибок.
	if (mode == FileReadMode::Binary && read != fileSize)
	{
		LOG_F_ERR("Ошибка чтения файла \"%s\" в бинарном виде. read != fileSize.\n"
				  "read = %zd, fileSize = %zd.",
				  fileName, read, fileSize);
		return TextError::FileRead;
	}

	fclose(file);

	text->Data   = buffer;
	text->Size   = BufferSize;

	return TextError::NoErrors;
}

void TextDestructor(Text* const text)
{
	assert(text);

	free(text->Data);
	memset(text, 0, sizeof(Text));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///