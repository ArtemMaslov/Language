#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "_identifier.h"
#include "../../Modules/ErrorsHandling.h"

#define CHECK_STATUS \
	if (status != ProgramStatus::Ok) \
		return status

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus IdentifierTableConstructor(IdentifierTable* table)
{
	assert(table);

	ProgramStatus status = ProgramStatus::Ok;
	status = ExtArrayConstructor(&table->Table, sizeof(Identifier));
	CHECK_STATUS;

	status = ExtHeapConstructor(&table->_identifierNames, IdentifierNamesDefaultSize);

	return status;
}

ProgramStatus IdentifierTableDestructor(IdentifierTable* table)
{
	assert(table);

	ProgramStatus status = ProgramStatus::Ok;
	status = ExtArrayDestructor(&table->Table);
	CHECK_STATUS;

	status = ExtHeapDestructor(&table->_identifierNames);

	memset(table, 0, sizeof(IdentifierTable));

	return status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus IdentifierTableAddElem(IdentifierTable* table, const char* name, const size_t nameLength, int* outId)
{
	assert(table);
	assert(name);
	assert(outId);

	const size_t tableSize = table->Table.Size;

	for (size_t st = 0; st < tableSize; st++)
	{
		const Identifier* idTable = (Identifier*)ExtArrayGetElemAt(&table->Table, st);

		const size_t minSize = nameLength < idTable->NameLength ? nameLength : idTable->NameLength;

		if (strncmp(name, idTable->Name, minSize) == 0)
		{
			*outId = idTable->Id;
			return ProgramStatus::Ok;
		}
	}

	const int id = (int)table->Table.Size;

	char* nameHeap = (char*)ExtHeapAllocElem(&table->_identifierNames, nameLength + 1);

	if (!nameHeap)
	{
		return ProgramStatus::Fault;
	}

	memcpy(nameHeap, name, nameLength);

	Identifier identifier = {};
	identifier.Name       = nameHeap;
	identifier.NameLength = nameLength;
	identifier.Id         = id;

	*outId = id;
	return ExtArrayAddElem(&table->Table, &identifier);
}

const Identifier* IdentifierGetById(const IdentifierTable* table, const int id)
{
	assert(table);
	
	const size_t identifiersCount = table->Table.Size;
	for (size_t st = 0; st < identifiersCount; st++)
	{
		const Identifier* identifier = (Identifier*)ExtArrayGetElemAt(&table->Table, st);

		if (identifier->Id == id)
			return identifier;
	}

	return nullptr;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#undef CHECK_STATUS