#ifndef INTRINSIC_FUNCTIONS_H
#define INTRINSIC_FUNCTIONS_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

struct IntrinsicFunction
{
	const char* Name;
	size_t      NameSize;
};

static const IntrinsicFunction IntrinSqrt = {"sqrt", sizeof("sqrt") - 1};

static const IntrinsicFunction IntrinInt = {"int", sizeof("int") - 1};

static const IntrinsicFunction IntrinLight = {"light", sizeof("light") - 1};

static const IntrinsicFunction IntrinDark = {"dark", sizeof("dark") - 1};

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !INTRINSIC_FUNCTIONS_H