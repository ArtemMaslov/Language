#ifndef IDENTIFIER_H
#define IDENTIFIER_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#include "../../Modules/ErrorsHandling.h"
#include "../../Modules/ExtArray/ExtArray.h"
#include "../../Modules/ExtHeap/ExtHeap.h"
#include "../../LanguageGrammar/LanguageGrammar.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const size_t IdentifierNamesDefaultSize = 4096;

struct Identifier
{
	int    Id;
	char*  Name;
	size_t NameLength;

	/// Используется на этапе компиляции. Показывает, где найти переменную в памяти.
	int CompileMemoryOffset;
};

struct IdentifierTable
{
	ExtArray Table;

	size_t CompileIdentInited;

	ExtHeap _identifierNames;
};

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus IdentifierTableConstructor(IdentifierTable* table);

ProgramStatus IdentifierTableDestructor(IdentifierTable* table);

ProgramStatus IdentifierTableAddElem(IdentifierTable* table, const char* name, const size_t nameLength, int* outId);

const Identifier* IdentifierGetById(const IdentifierTable* table, const int id);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !IDENTIFIER_H