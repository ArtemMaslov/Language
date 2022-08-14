#ifndef INTRINSIC_FUNCTIONS_H
#define INTRINSIC_FUNCTIONS_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

struct IntrinsicFunction
{
	const char* Name;
	size_t      NameSize;
};

static const IntrinsicFunction IntrinSqrt = {"sqrt", 4};

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !INTRINSIC_FUNCTIONS_H