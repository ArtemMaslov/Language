#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ExtHeap.h"
#include "../ErrorsHandling.h"

static ProgramStatus ExtHeapResize(ExtHeap* extHeap);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus ExtHeapConstructor(ExtHeap* extHeap, const size_t capacity)
{
	assert(extHeap);

	char* heap = (char*)calloc(capacity, sizeof(char));

	if (!heap)
	{
		return ProgramStatus::Fault;
	}

	extHeap->Heap     = heap;
	extHeap->Size     = 0;
	extHeap->Capacity = capacity;

	return ProgramStatus::Ok;
}

ProgramStatus ExtHeapDestructor(ExtHeap* extHeap)
{
	assert(extHeap);

	free(extHeap->Heap);
	memset(extHeap, 0, sizeof(ExtHeap));

	return ProgramStatus::Ok;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

void* ExtHeapAllocElem(ExtHeap* extHeap, const size_t elemSize)
{
	assert(extHeap);

	assert(elemSize > 0);

	const size_t size = extHeap->Size;

	while (size + elemSize > extHeap->Capacity)
	{
		ProgramStatus status = ExtHeapResize(extHeap);
		if (status != ProgramStatus::Ok)
			return nullptr;
	}

	extHeap->Size += elemSize;

	return extHeap->Heap + size;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static ProgramStatus ExtHeapResize(ExtHeap* extHeap)
{
	assert(extHeap);

	size_t newCapacity = (size_t)((double)extHeap->Capacity * EXT_HEAP_RESIZE_MULTIPLIER);

	char* resized = (char*)realloc(extHeap->Heap, newCapacity);

	if (!resized)
		return ProgramStatus::Fault;

	extHeap->Heap     = resized;
	extHeap->Capacity = newCapacity;

	return ProgramStatus::Ok;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 