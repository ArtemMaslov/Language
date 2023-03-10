///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль компилятора для архитектуры эмулятора процессора SoftCpu.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 19:11 14.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef SOFT_CPU_COMPILER_H
#define SOFT_CPU_COMPILER_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include "../../AST/AST.h"
#include "../../Modules/ErrorsHandling.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Скомпилировать программу для архитектуры SoftCpu.
 * 
 * @param ast     Указатель на заполненное АСД.
 * @param outFile Имя выходного файла с ассемблерным кодом SoftCpu.
 * 
 * @return ProgramStatus::Ok, если функция выполнена без ошибок.
 */
ProgramStatus SoftCpuCompileFile(AST* ast, const char* outFile);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !SOFT_CPU_COMPILER_H