#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ExtArray.h"
#include "../Logs/Logs.h"
#include "../ErrorsHandling.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ProgramStatus ExtArrayResize(ExtArray* extArray);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus ExtArrayConstructor(ExtArray* extArray, const size_t elemSize, const size_t capacity)
{
	assert(extArray);

	assert(elemSize != 0);
	assert(capacity != 0);

	void* data = calloc(capacity, elemSize);

	if (!data)
		return ProgramStatus::Fault;

	extArray->Array = data;
	extArray->ElemSize = elemSize;

	extArray->Size     = 0;
	extArray->Capacity = capacity;

	return ProgramStatus::Ok;
}

ProgramStatus ExtArrayDestructor(ExtArray* extArray)
{
	assert(extArray);

	free(extArray->Array);
	memset(extArray, 0, sizeof(ExtArray));

	return ProgramStatus::Ok;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus ExtArrayAddElem(ExtArray* extArray, const void* elem)
{
	assert(extArray);

	ProgramStatus status = ProgramStatus::Ok;

	const size_t size     = extArray->Size;
	const size_t capacity = extArray->Capacity;
	const size_t elemSize = extArray->ElemSize;

	assert(size <= capacity);
	if (size == capacity)
	{
		status = ExtArrayResize(extArray);
		if (status != ProgramStatus::Ok)
			return status;
	}

	memcpy((char*)extArray->Array + size * elemSize, elem, elemSize);
	extArray->Size++;

	return status;
}

void* ExtArrayGetElemAt(const ExtArray* extArray, const size_t index)
{
	assert(extArray);

	assert(index < extArray->Size);

	return (char*)extArray->Array + index * extArray->ElemSize;
}

static ProgramStatus ExtArrayResize(ExtArray* extArray)
{
	assert(extArray);

	size_t newCapacity = (size_t)((double)extArray->Capacity * EXT_ARRAY_RESIZE_MULTIPLIER);
	size_t newDataSize = newCapacity * extArray->ElemSize;

	void* resized = realloc(extArray->Array, newDataSize);

	if (!resized)
		return ProgramStatus::Fault;

	extArray->Array    = resized;
	extArray->Capacity = newCapacity;

	return ProgramStatus::Ok;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 