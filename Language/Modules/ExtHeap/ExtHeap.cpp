///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль выделителя памяти расширяющаяся куча ExtHeap.
// 
// Файлы с исходным кодом.
// 
// Версия: 1.0.0.3
// Дата последнего изменения: 17:12 28.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../ErrorsHandling.h"
#include "../Logs/Logs.h"

#include "ExtHeap.h"

#if MODULE_EXT_HEAP_DISABLE_LOGS
#include "../DisableLogs.h"
#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Выделить новый блок памяти, больший старого.
 * 
 * @param extHeap Указатель на структуру ExtHeap.
 * 
 * @return ProgramStatus::Fault, если память выделить не удалось, при этом состояние ExtHeap не меняется.
 *         ProgramStatus::Ok, если нет ошибок.
*/
static ProgramStatus ExtHeapResize(ExtHeap* const extHeap);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus ExtHeapConstructor(ExtHeap* const extHeap, const size_t capacity, const float resize)
{
	assert(extHeap);
	assert(capacity > 0);
	assert(resize   > 1);

	char* heap = (char*)calloc(capacity, sizeof(char));
	if (!heap)
	{
		LOG_ERR_MEMORY(capacity * sizeof(char));
		return ProgramStatus::Fault;
	}

	extHeap->Heap           = heap;
	extHeap->Size           = 0;
	extHeap->Capacity       = capacity;
	extHeap->Resize         = resize;
	extHeap->ReallocCounter = 0;

	return ProgramStatus::Ok;
}

#undef ExtHeapDestructor
void ExtHeapDestructor(ExtHeap* const extHeap, 
					   const char* const funct, const char* const file, const size_t line)
{
	assert(extHeap);
	assert(funct);
	assert(file);

	if (extHeap->ReallocCounter > 0)
	{
		LOG_F_WRN("В процессе работы ExtHeap были перевыделения памяти.\n"
				  "Не эффективный начальный размер capacity.\n"
				  "Функция ExtHeapDestructor вызвана %s из файла %s в строке %zd.",
				  funct, file, line);
		ExtHeapDump(extHeap);
	}

	free(extHeap->Heap);
	memset(extHeap, 0, sizeof(ExtHeap));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

void* ExtHeapAllocElem(ExtHeap* const extHeap, const size_t elemSize)
{
	assert(extHeap);
	assert(elemSize > 0);

	const size_t size = extHeap->Size;

	// Закончилась память устройства.
	if (size + elemSize < size)
	{
		LOG_F_ERR("Ошибка выделения памяти. size + elemSize < size. size = %zd, elemSize = %zd.", size, elemSize);
		return nullptr;
	}

	// Увеличиваем размер блока памяти до тех пор, пока он не сможет вместить
	// добавляемый элемент. Обычно это делается за одно выделение. Если коэффициент
	// выделения памяти настроен не эффективно, то выделений будет несколько, что
	// замедляет работу.
	while (size + elemSize > extHeap->Capacity)
	{
		LOG_F_WRN("Выделение дополнительный памяти ExtHeap. Не эффективный начальный размер capacity. size = %zd, elemSize = %zd.",
				  size, elemSize);
		ExtHeapDump(extHeap);

		ProgramStatus status = ExtHeapResize(extHeap);
		if (status == ProgramStatus::Fault)
		{
			TRACE_ERROR();
			return nullptr;
		}

		// Проверяем, что выделение новой памяти позволяет вместить добавляемый элемент.
		// Иначе предупреждаем пользователя, что коэффициент выделения настроен не эффективно.
		if (size + elemSize > extHeap->Capacity)
		{
			LOG_F_WRN("Не эффективный коэффициент выделения памяти в ExtHeap. size = %zd, elemSize = %zd.",
					  size, elemSize);
			ExtHeapDump(extHeap);
		}
	}

	extHeap->Size += elemSize;

	return extHeap->Heap + size;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static ProgramStatus ExtHeapResize(ExtHeap* const extHeap)
{
	assert(extHeap);
	assert(extHeap->Capacity > 0);
	assert(extHeap->Resize   > 1);

	size_t newCapacity = (size_t)((float)extHeap->Capacity * extHeap->Resize);

	char* resized = (char*)realloc(extHeap->Heap, newCapacity);
	if (!resized)
	{
		LOG_ERR_MEMORY(newCapacity);
		return ProgramStatus::Fault;
	}

	// Если выделение памяти произошло с ошибкой, то состояние структуры не меняется.
	extHeap->Heap     = resized;
	extHeap->Capacity = newCapacity;
	extHeap->ReallocCounter++;

	return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#undef ExtHeapDump
void ExtHeapDump(const ExtHeap* const extHeap,
				 const char* const funct, const char* const file, const size_t line)
{
	assert(extHeap);
	assert(funct);
	assert(file);

	LOG_F_DBG("Состояние ExtHeap [%p]:\n"
			  "Heap           = %p;\n"
			  "Size           = %zd;\n"
			  "Capacity       = %zd;\n"
			  "Resize         = %f;\n"
			  "ReallocCounter = %zd.\n"
			  "Функция ExtHeapDump вызвана %s из файла %s в строке %zd.",
			  extHeap, extHeap->Heap, extHeap->Size, extHeap->Capacity, extHeap->Resize,
			  extHeap->ReallocCounter, funct, file, line);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///