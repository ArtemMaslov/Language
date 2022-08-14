#ifndef EXTENDING_HEAP_H
#define EXTENDING_HEAP_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#include "../ErrorsHandling.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const double EXT_HEAP_RESIZE_MULTIPLIER = 2;
const size_t EXT_HEAP_DEFAULT_CAPACITY  = 4096;

struct ExtHeap
{
	char*  Heap;

	size_t Size;
	size_t Capacity;
};

ProgramStatus ExtHeapConstructor(ExtHeap* extHeap, const size_t capacity = EXT_HEAP_DEFAULT_CAPACITY);

ProgramStatus ExtHeapDestructor(ExtHeap* extHeap);

void* ExtHeapAllocElem(ExtHeap* extHeap, const size_t elemSize);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !EXTENDING_HEAP_H