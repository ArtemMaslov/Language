///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Встроенные в язык функции.
// 
// Версия: 1.0.0.0
// Дата последнего изменения: 18:27 14.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef INTRINSIC_FUNCTIONS_H
#define INTRINSIC_FUNCTIONS_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Структура описания встроенной функции.
struct IntrinsicFunction
{
	/// Имя функции.
	const char* Name;
	/// Длина имени.
	size_t      NameSize;
};

/// sqrt(x) - извлечь из числа x арифметический квадратный корень.
static const IntrinsicFunction IntrinSqrt = {"sqrt", sizeof("sqrt") / sizeof(char) - 1};

/// set_ram(addr, value) - записать значение value в оперативную память по адресу addr.
static const IntrinsicFunction IntrinSetRam = {"set_ram", sizeof("set_ram") / sizeof(char) - 1};

/// value = get_ram(addr) - прочитать значение из оперативной памяти по адресу addr.
static const IntrinsicFunction IntrinGetRam = {"get_ram", sizeof("get_ram") / sizeof(char) - 1};

/// dsp() - вывести на экран содержимое видео-памяти.
static const IntrinsicFunction IntrinDisplay = {"dsp", sizeof("dsp") / sizeof(char) - 1};

/// int(x) - преобразовать число x из double в int.
static const IntrinsicFunction IntrinInt = {"int", sizeof(int) / sizeof(char) - 1};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !INTRINSIC_FUNCTIONS_H