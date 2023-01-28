#ifndef X86_COMPILER_H
#define X86_COMPILER_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include "../../AST/AST.h"
#include "../../Modules/ErrorsHandling.h"

#include "_asm_representation.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

struct x86Compiler
{
	size_t LabelIndex;

	AST*   Ast;
	AsmRep AsmRep;

	/// Узел функции, которая сейчас компилируется.
	const FunctDefNode* CurrentFunction;

	IdentifierTable CurrentVariablesScope;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus x86CompilerConstructor(x86Compiler* const comp, AST* const ast);

ProgramStatus x86WriteListingFile(const x86Compiler* const comp, const char* const fileName, AsmListingType type);

void x86CompilerDestructor(x86Compiler* const comp);

ProgramStatus x86CompileAsmRep(x86Compiler* const comp);

ProgramStatus AsmTranslateCommands(x86Compiler* const comp);

ProgramStatus x86CompileJitBuffer(x86Compiler* const comp);

ProgramStatus x86RunJIT(x86Compiler* const comp);

ProgramStatus x86DumpJIT(x86Compiler* const comp, const char* const fileName);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !X86_COMPILER_H