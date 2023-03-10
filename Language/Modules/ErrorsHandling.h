///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль обработки ошибок программы.
// 
// Версия: 1.0.0.0
// Дата последнего изменения: 12:37 28.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef ERRORS_HANDLING_H
#define ERRORS_HANDLING_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

enum class ProgramStatus
{
	/// Функция успешно завершила свою работу
	Ok = 0,

	/// Предупреждение. Возникла не критическая особая ситуация во время выполнения программы.
	Warning = 1,

	/// Критическая ошибка. Необходимо завершить выполнения программы.
	Fault = 2,

	/// Ещё не реализовано.
	/// Возвращается, если функция ещё не была реализована.
	NotImplemented = 3
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !ERRORS_HANDLING_H