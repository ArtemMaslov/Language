#ifndef LIST_CONFIG_H
#define LIST_CONFIG_H

#include <string.h>
#include <emmintrin.h>
#include <smmintrin.h>

#include "../FilesIO/FilesIO.h"

#include "../Text/Text.h"
#include "List.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

/// @brief Тип элементов списка
/// Заменить на тип элементов, которые будет использовать список
typedef int ListType;

/// Папка, в которую будут записаны изображения при графическом дампе.
#define LIST_LOG_IMAGE_FOLDER "_listImgs\\"

/// Включить проверку состояния списка с помощью assert.
#define LIST_ENABLE_ASSERT_STATUS true

#if LIST_ENABLE_ASSERT_STATUS
/// Проверить состояние списка.
#define LIST_ASSERT_STATUS assert(list->Status == LIST_ERR_NO_ERRORS)
#else
/// Проверить состояние списка.
#define LIST_ASSERT_STATUS ((void)0)
#endif

/// Коэффициент, на который умножается вместительность списка при расширении.
const double ListResizeCoef = 2;

/**
 * @brief      Сравнивает два элемента списка.
 * 
 * @param val1 Первый элемент.
 * @param val2 Второй элемент.
 * 
 * @return     true, если элементы равны. false, если элементы различны.
*/
static inline bool ListComparator(const ListType val1, const ListType val2)
{
	return memcmp(&val1, &val2, sizeof(ListType)) == 0;
}

/// Размер буфера преобразования элемента в строку.
const size_t ListToStringSize = 32;

/**
 * @brief        Преобразовать в строку элемент списка.
 * 
 * @param buffer Буфер выходной строки.
 * @param value  Элемент списка.
*/
static inline void ListElementToString(char* buffer, const size_t bufferSize, const ListType* value)
{
	snprintf(buffer, bufferSize, "%d", *value);

	buffer[bufferSize - 1] = '\0';
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !LIST_CONFIG_H