///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль работы с файловой системой.
// 
// Файл с исходным кодом.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 20:44 03.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>

#include "../Logs/Logs.h"
#if MODULE_FILES_IO_DISABLE_LOGS
#include "../DisableLogs.h"
#endif

#include "../TargetOS.h"

#include "FilesIO.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#if defined(WINDOWS)

#include <windows.h>

FilesIoError DirectoryExist(const char* const path)
{
	assert(path);

	DWORD attr = GetFileAttributesA(path);

	if (attr == INVALID_FILE_ATTRIBUTES)
		return FilesIoError::NotExist;

	if (attr & FILE_ATTRIBUTE_DIRECTORY)
		return FilesIoError::Exist;
	else
		return FilesIoError::NotExist;
}

FilesIoError DirectoryCreate(const char* const path)
{
	if (!CreateDirectoryA(path, nullptr))
		return FilesIoError::DirectoryCreate;
	return FilesIoError::NoErrors;
}

#elif defined(LINUX)

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

FilesIoError DirectoryExist(const char* const path)
{
    assert(path);

	struct stat statInfo = {};
	if (stat(path, &statInfo) == -1) 
		return FilesIoError::NotExist;
	else
		return FilesIoError::Exist;
}

FilesIoError DirectoryCreate(const char* const path)
{
	assert(path);

    if (mkdir(path, 0766) == -1)
		return FilesIoError::DirectoryCreate;
	
	return FilesIoError::NoErrors;
}

#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

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