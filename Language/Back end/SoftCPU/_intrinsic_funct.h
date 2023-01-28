#ifndef INTRINSIC_FUNCTIONS_H
#define INTRINSIC_FUNCTIONS_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

struct IntrinsicFunction
{
	const char* Name;
	size_t      NameSize;
};

static const IntrinsicFunction IntrinSqrt = {"sqrt", sizeof("sqrt") / sizeof(char) - 1};

static const IntrinsicFunction IntrinSetRam = {"set_ram", sizeof("set_ram") / sizeof(char) - 1};

static const IntrinsicFunction IntrinGetRam = {"get_ram", sizeof("get_ram") / sizeof(char) - 1};

static const IntrinsicFunction IntrinDisplay = {"dsp", sizeof("dsp") / sizeof(char) - 1};

static const IntrinsicFunction IntrinInt = {"int", sizeof(int) / sizeof(char) - 1};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !INTRINSIC_FUNCTIONS_H