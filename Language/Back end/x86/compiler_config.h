///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора в архитектуру Intel x86 (32-разрядная). 
//
// Конфигурация.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 19:00 08.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef COMPILER_CONFIG_H
#define COMPILER_CONFIG_H

/// Значение нулевого идентификатора.
const size_t IdentifierNull  = (size_t)(-1);
/// Для каждой метки (символьное представление) выделяется столько байт памяти.
const size_t LabelBufferSize = 32;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***/// 
/// 								   Condition
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***/// 

const char* const CompEqualLabelName        = "equal";
const char* const CompNotEqualLabelName     = "not_equal";
const char* const CompLessLabelName         = "less";
const char* const CompLessEqualLabelName    = "less_equal";
const char* const CompGreaterLabelName      = "greater";
const char* const CompGreaterEqualLabelName = "greater_equal";

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !COMPILER_CONFIG_H