///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль выделителя памяти расширяющийся массив ExtArray.
// 
// Файлы с исходным кодом.
// 
// Версия: 1.0.0.1
// Дата последнего изменения: 17:38 28.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../ErrorsHandling.h"
#include "../Logs/Logs.h"

#include "ExtArray.h"

#if MODULE_EXT_ARRAY_DISABLE_LOGS
#include "../DisableLogs.h"
#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief Увеличить размер массива.
 * 
 * @param extArray Указатель на ExtArray.
 * 
 * @return ProgramStatus::Fault, если не удалось выделить память.
 *         ProgramStatus::Ok, если нет ошибок.
*/
static ProgramStatus ExtArrayResize(ExtArray* const extArray);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus ExtArrayConstructor(ExtArray* const extArray, const size_t elemSize, const size_t capacity, const float resize)
{
	assert(extArray);
	assert(elemSize > 0);
	assert(capacity > 0);
	assert(resize   > 1);

	void* data = calloc(capacity, elemSize);
	if (!data)
	{
		LOG_ERR_MEMORY(capacity * elemSize);
		return ProgramStatus::Fault;
	}

	extArray->Array          = data;
	extArray->ElemSize       = elemSize;
	extArray->Size           = 0;
	extArray->Capacity       = capacity;
	extArray->Resize         = resize;
	extArray->ReallocCounter = 0;

	return ProgramStatus::Ok;
}

#undef ExtArrayDestructor
void ExtArrayDestructor(ExtArray* const extArray,
						const char* const funct, const char* const file, const size_t line)
{
	assert(extArray);
	assert(funct);
	assert(file);

	if (extArray->ReallocCounter > 0)
	{
		LOG_F_WRN("В процессе работы ExtArray были перевыделения памяти.\n"
				  "Не эффективный начальный размер capacity.\n"
				  "Функция ExtArrayDestructor вызвана %s из файла %s в строке %zd.",
				  funct, file, line);
		ExtArrayDump(extArray);
	}

	free(extArray->Array);
	memset(extArray, 0, sizeof(ExtArray));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus ExtArrayAddElem(ExtArray* const extArray, const void* const elem)
{
	assert(extArray);
	assert(elem);

	const size_t size     = extArray->Size;
	const size_t capacity = extArray->Capacity;
	const size_t elemSize = extArray->ElemSize;

	if (size == capacity)
	{
		LOG_WRN("Выделение дополнительный памяти ExtArray. Не эффективный начальный размер capacity.");
		ExtArrayDump(extArray);

		ProgramStatus status = ExtArrayResize(extArray);
		if (status == ProgramStatus::Fault)
		{
			TRACE_ERROR();
			return status;
		}
	}

	memcpy((char*)extArray->Array + size * elemSize, elem, elemSize);
	extArray->Size++;

	return ProgramStatus::Ok;
}

void* ExtArrayGetElemAt(const ExtArray* const extArray, const size_t index)
{
	assert(extArray);
	assert(index < extArray->Size);

	return (char*)extArray->Array + index * extArray->ElemSize;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static ProgramStatus ExtArrayResize(ExtArray* const extArray)
{
	assert(extArray);
	assert(extArray->Capacity > 0);
	assert(extArray->Resize   > 1);

	// Новое количество элементов массива.
	size_t newCapacity = (size_t)((float)extArray->Capacity * extArray->Resize);
	// Новый размер блока памяти в байтах.
	size_t newDataSize = newCapacity * extArray->ElemSize;

	void* resized = realloc(extArray->Array, newDataSize);
	if (!resized)
	{
		LOG_ERR_MEMORY(newDataSize);
		return ProgramStatus::Fault;
	}

	// Если выделение памяти произошло с ошибкой, то состояние структуры не меняется.
	extArray->Array    = resized;
	extArray->Capacity = newCapacity;
	extArray->ReallocCounter++;

	return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#undef ExtArrayDump
void ExtArrayDump(const ExtArray* const extArray,
				  const char* const funct, const char* const file, const size_t line)
{
	assert(extArray);
	assert(funct);
	assert(file);

	LOG_F_DBG("Состояние ExtArray [%p]:\n"
			  "Array          = %p;\n"
			  "ElemSize       = %zd;\n"
			  "Size           = %zd;\n"
			  "Capacity       = %zd;\n"
			  "Resize         = %f;\n"
			  "ReallocCounter = %zd.\n"
			  "Функция ExtArrayDump вызвана %s из файла %s в строке %zd.",
			  extArray, extArray->Array, extArray->ElemSize, extArray->Size, extArray->Capacity, 
			  extArray->Resize, extArray->ReallocCounter, funct, file, line);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///