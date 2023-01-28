///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// ������ ������ ������.
// 
// ���� � �������� �����.
// 
// ������: 1.0.0.2
// ���� ���������� ���������: 17:54 28.01.2023
// 
// �����: ������ �.�. (https://github.com/ArtemMaslov).
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\

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
		assert(!"������ �������� �����");
		LOG_F_ERR("������ �������� ����� \"%s\".", fileName);
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

	// ���� ���� �������� ��� ���������, �� read <= fileSize, ��� ���, ��������, '\r\n' ���������� �� '\n'.
	// ���� ���� �������� ��� ��������, �� read == fileSize, � ������ ������ ����� ��� ������.
	if (mode == FileReadMode::Binary && read != fileSize)
	{
		LOG_F_ERR("������ ������ ����� \"%s\" � �������� ����. read != fileSize.\n"
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