#include <stdio.h>
#include <stdlib.h>

#include "List.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

//#define LIST_INT_TYPE

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#define CLEAR_AND_RETURN goto clear_and_return;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#ifdef LIST_INT_TYPE

void ListUnitTest_BasicFunctions1()
{
	List   list      = {};
	size_t listIndex = 0;

	if (ListConstructor(&list, 30) != LIST_ERR_NO_ERRORS)
		CLEAR_AND_RETURN;

	for (; listIndex < 20; listIndex++)
	{
		int value = 1000 + listIndex;

		if (ListAddElemAfter(&list, &value, listIndex) != LIST_ERR_NO_ERRORS)
			CLEAR_AND_RETURN;
	}

	ListRemoveElem(&list, 2);
	ListRemoveElem(&list, 4);
	ListRemoveElem(&list, 6);

	ListRemoveElem(&list, 11);
	ListRemoveElem(&list, 12);
	ListRemoveElem(&list, 13);

	ListGraphicDump(&list);

	for (size_t st = 0; st < 5; st++)
	{
		int value = 500 + st;

		if (ListAddElemAfter(&list, &value, listIndex) != LIST_ERR_NO_ERRORS)
			CLEAR_AND_RETURN;
	}

	ListGraphicDump(&list);

clear_and_return:
	ListDestructor(&list);
}

void ListUnitTest_Resize1()
{
	List   list      = {};
	size_t listIndex = 0;

	if (ListConstructor(&list, 5) != LIST_ERR_NO_ERRORS)
		CLEAR_AND_RETURN;

	for (; listIndex < 20; listIndex++)
	{
		int value = 1000 + listIndex;

		if (ListAddElemAfter(&list, &value, listIndex) != LIST_ERR_NO_ERRORS)
			CLEAR_AND_RETURN;
	}

	ListGraphicDump(&list);

clear_and_return:
	ListDestructor(&list);
}

#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\