///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль работы с файловой системой.
// 
// Использование:
// 1. Добавить директорию "Modules/FilesIO" в проект.
// 
// 2. Подключить заголовочный файл "FilesIO.h".
// 
// 3. Доступные функции (параметры и описание см. ниже):
//     4.1.  DirectoryExist
//     4.2.  GetFileSize
// 
// 4. Чтобы отключить поддержку логов нужно добавить в проект файл "Modules/DisableLogs.h"
//    и в этом файле установить значение true дефайна MODULE_FILES_IO_DISABLE_LOGS.
//
// Версия: 1.0.2.0
// Дата последнего изменения: 20:44 03.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef FILES_IO_H
#define FILES_IO_H

#include <stdio.h>

#include "../TargetOS.h"

/// Чтобы отключить поддержку логов в модуле, нужно установить значение true.
#define MODULE_FILES_IO_DISABLE_LOGS false

#if defined(WINDOWS)
#define PATH_SEPARATOR "\\"
#elif defined(LINUX)
#define PATH_SEPARATOR "/"
#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Особые ситуации работы модуля FilesIO.
enum class FilesIoError
{
    /// Нет ошибок.
    NoErrors,
    /// Директория существует.
    Exist,
    /// Директория не существует.
    NotExist,
    /// Ошибка создания директории.
    DirectoryCreate
};

/**
 * @brief  Проверяет, существует ли директория.
 *
 * @param path Путь к директории.
 *
 * @return FilesIoError::Exist, если директория существует.
 *         FilesIoError::NotExist, если директория не существует.
*/
FilesIoError DirectoryExist(const char* const path);

/**
 * @brief  Создать директорию.
 * 
 * @param path Путь к директории.
 * 
 * @return FilesIoError::CreateDirectory,
 *         FilesIoError::NoErrors.
 */
FilesIoError DirectoryCreate(const char* const path);

/**
 * @brief  Получить размер файла.
 *
 * @param file Указатель на открытый файл.
 *
 * @return Размер файла.
*/
size_t GetFileSize(FILE* const file);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !FILES_IO_H