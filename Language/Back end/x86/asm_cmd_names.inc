///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Команды процессора архитектуры Intel x86 (32-разряда).
//
// Внимание! Данный файл используется, для того, чтобы команды в enum class AsmCmdType не повторялись.
// 
// Версия: 1.0.0.0
// Дата последнего изменения: 19:59 13.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ASM_CMD(push,     "push")
ASM_CMD(pop,      "pop")
ASM_CMD(mov,      "mov")

ASM_CMD(add,      "add")
ASM_CMD(sub,      "sub")
ASM_CMD(mul,      "mul")
ASM_CMD(imul,     "imul")
ASM_CMD(div,      "div")
ASM_CMD(idiv,     "idiv")

ASM_CMD(xor_,     "xor")
ASM_CMD(neg,      "neg")

ASM_CMD(cdq,      "cdq")

ASM_CMD(cmp,      "cmp")

ASM_CMD(jmp,      "jmp")
ASM_CMD(je,       "je")
ASM_CMD(jne,      "jne")
ASM_CMD(ja,       "ja")
ASM_CMD(jae,      "jae")
ASM_CMD(jb,       "jb")
ASM_CMD(jbe,      "jbe")

ASM_CMD(jg,       "jg")
ASM_CMD(jge,      "jge")
ASM_CMD(jl,       "jl")
ASM_CMD(jle,      "jle")

ASM_CMD(call,     "call")
ASM_CMD(ret,      "ret")

ASM_CMD(fchs,     "fchs")
ASM_CMD(fld,      "fld")
ASM_CMD(fstp,     "fstp")
ASM_CMD(fstpd,    "fstpd")
ASM_CMD(faddp,    "faddp")
ASM_CMD(fsubp,    "fsubp")
ASM_CMD(fmulp,    "fmulp")
ASM_CMD(fdivp,    "fdivp")
ASM_CMD(fcompp,   "fcompp")
ASM_CMD(frnd,     "frnd")
ASM_CMD(fistp,    "fistp")