///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// ������ ������ � �������� ��������.
// 
// ���� � �������� �����.
// 
// ������: 1.0.0.0
// ���� ���������� ���������: 12:02 28.01.2023
// 
// �����: ������ �.�. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>

#include <windows.h>

#include "FilesIO.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

bool DirectoryExist(const char* const path)
{
	DWORD attr = GetFileAttributesA(path);

	if (attr == INVALID_FILE_ATTRIBUTES)
		return false;

	return (attr & FILE_ATTRIBUTE_DIRECTORY);
}

size_t GetFileSize(FILE* const file)
{
	assert(file);

	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	return fileSize;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///