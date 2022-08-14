#ifndef EXT_ARRAY_H
#define EXT_ARRAY_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#include "../ErrorsHandling.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\  
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\  

const double EXT_ARRAY_RESIZE_MULTIPLIER = 2;
const size_t EXT_ARRAY_DEFAULT_CAPACITY  = 128;

struct ExtArray
{
	void*  Array;

	size_t ElemSize;

	size_t Size;
	size_t Capacity;
};

ProgramStatus ExtArrayConstructor(ExtArray* extArray, const size_t elemSize, const size_t capacity = EXT_ARRAY_DEFAULT_CAPACITY);

ProgramStatus ExtArrayDestructor(ExtArray* extArray);

ProgramStatus ExtArrayAddElem(ExtArray* extArray, const void* elem);

void* ExtArrayGetElemAt(const ExtArray* extArray, const size_t index);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !EXT_ARRAY_H