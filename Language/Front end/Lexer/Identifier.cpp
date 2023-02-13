///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль лексического анализатора.
// 
// Версия: 1.0.1.0
// Дата последнего изменения: 20:43 29.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../Modules/Logs/Logs.h"
#include "../../Modules/ErrorsHandling.h"

#include "Identifier.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Ищет идентификатор в таблице по его символьному представлению.
 * 
 * Функция приватная для внешней программы, так как работает медленнее, чем IdentifierGetById.
 * Внешней программе нужно хранить номера идентификаторов, а не их символьные представления.
 * 
 * @param table      Указатель на таблице идентификаторов.
 * @param name       Символьное представление идентификатора.
 * @param nameLength Длина символьного представления.
 * @param out_id     Выходной номер идентификатора.
 * 
 * @return true, если идентификатор был найден в таблице.
 *         false, если идентификатор не был найден в таблице.
*/
static bool IdentifierGetByName(const IdentifierTable* const table, const char* const name,
								const size_t nameLength, size_t* const out_id);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

IdentifierError IdentifierTableConstructor(IdentifierTable* const table)
{
	assert(table);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayConstructor(&table->Records, sizeof(Identifier));
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return IdentifierError::ExtArray;
	}

	status = ExtHeapConstructor(&table->IdentifierNames, IdentifierNamesDefaultSize);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		ExtArrayDestructor(&table->Records);
		return IdentifierError::ExtHeap;
	}

	return IdentifierError::NoErrors;
}

void IdentifierTableDestructor(IdentifierTable* const table)
{
	assert(table);

	ExtHeapDestructor(&table->IdentifierNames);
	ExtArrayDestructor(&table->Records);
	memset(table, 0, sizeof(IdentifierTable));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

IdentifierError IdentifierTableAddElem(IdentifierTable* const table, const char* const name,
									   const size_t nameLength, size_t* const out_id)
{
	assert(table);
	assert(name);
	assert(nameLength > 0);
	assert(out_id);

	// Идентификатор уже есть в таблице.
	if (IdentifierGetByName(table, name, nameLength, out_id))
		return IdentifierError::NoErrors;

	// Добавляем идентификатор в таблице.
	// Уникальными номерами идентификаторов являются их индексы в таблице.
	const size_t id = (size_t)table->Records.Size;

	char* nameHeap = (char*)ExtHeapAllocElem(&table->IdentifierNames, nameLength + 1);
	if (!nameHeap)
	{
		TRACE_ERROR();
		return IdentifierError::ExtHeapAddElem;
	}

	memcpy(nameHeap, name, nameLength);
	Identifier identifier = {};
	identifier.Name       = nameHeap;
	identifier.NameLength = nameLength;
	identifier.Id         = id;
	*out_id               = id;

    ProgramStatus status = ExtArrayAddElem(&table->Records, &identifier);

	assert(status != ProgramStatus::NotImplemented);
	if (status == ProgramStatus::Fault)
	{
		TRACE_ERROR();
		return IdentifierError::ExtArrayAddElem;
	}

	return IdentifierError::NoErrors;
}

const Identifier* IdentifierGetById(const IdentifierTable* const table, const size_t id)
{
	assert(table);
	// Указан не существующий номер идентификатора.
	assert(id < table->Records.Size);

	return (Identifier*)ExtArrayGetElemAt(&table->Records, id);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static bool IdentifierGetByName(const IdentifierTable* const table, const char* const name,
								const size_t nameLength, size_t* const out_id)
{
	assert(table);
	assert(name);
	assert(out_id);

	const size_t identifiersCount = table->Records.Size;
	for (size_t st = 0; st < identifiersCount; st++)
	{
		const Identifier* const identifier = (Identifier*)ExtArrayGetElemAt(&table->Records, st);

		if (identifier->NameLength == nameLength && strncmp(name, identifier->Name, nameLength) == 0)
		{
			*out_id = identifier->Id;
			return true;
		}
	}

	return false;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///