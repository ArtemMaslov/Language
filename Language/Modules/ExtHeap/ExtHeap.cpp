///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// ������ ���������� ������ ������������� ���� ExtHeap.
// 
// ����� � �������� �����.
// 
// ������: 1.0.0.3
// ���� ���������� ���������: 17:12 28.01.2023
// 
// �����: ������ �.�. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../ErrorsHandling.h"
#include "../Logs/Logs.h"

#include "ExtHeap.h"

#if MODULE_EXT_HEAP_DISABLE_LOGS
#include "../DisableLogs.h"
#endif

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  �������� ����� ���� ������, ������� �������.
 * 
 * @param extHeap ��������� �� ��������� ExtHeap.
 * 
 * @return ProgramStatus::Fault, ���� ������ �������� �� �������, ��� ���� ��������� ExtHeap �� ��������.
 *         ProgramStatus::Ok, ���� ��� ������.
*/
static ProgramStatus ExtHeapResize(ExtHeap* const extHeap);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus ExtHeapConstructor(ExtHeap* const extHeap, const size_t capacity, const float resize)
{
	assert(extHeap);
	assert(capacity > 0);
	assert(resize   > 1);

	char* heap = (char*)calloc(capacity, sizeof(char));
	if (!heap)
	{
		LOG_ERR_MEMORY(capacity * sizeof(char));
		return ProgramStatus::Fault;
	}

	extHeap->Heap           = heap;
	extHeap->Size           = 0;
	extHeap->Capacity       = capacity;
	extHeap->Resize         = resize;
	extHeap->ReallocCounter = 0;

	return ProgramStatus::Ok;
}

#undef ExtHeapDestructor
void ExtHeapDestructor(ExtHeap* const extHeap, 
					   const char* const funct, const char* const file, const size_t line)
{
	assert(extHeap);
	assert(funct);
	assert(file);

	if (extHeap->ReallocCounter > 0)
	{
		LOG_F_WRN("� �������� ������ ExtHeap ���� ������������� ������.\n"
				  "�� ����������� ��������� ������ capacity.\n"
				  "������� ExtHeapDestructor ������� %s �� ����� %s � ������ %zd.",
				  funct, file, line);
		ExtHeapDump(extHeap);
	}

	free(extHeap->Heap);
	memset(extHeap, 0, sizeof(ExtHeap));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

void* ExtHeapAllocElem(ExtHeap* const extHeap, const size_t elemSize)
{
	assert(extHeap);
	assert(elemSize > 0);

	const size_t size = extHeap->Size;

	// ����������� ������ ����������.
	if (size + elemSize < size)
	{
		LOG_F_ERR("������ ��������� ������. size + elemSize < size. size = %zd, elemSize = %zd.", size, elemSize);
		return nullptr;
	}

	// ����������� ������ ����� ������ �� ��� ���, ���� �� �� ������ ��������
	// ����������� �������. ������ ��� �������� �� ���� ���������. ���� �����������
	// ��������� ������ �������� �� ����������, �� ��������� ����� ���������, ���
	// ��������� ������.
	while (size + elemSize > extHeap->Capacity)
	{
		LOG_F_WRN("��������� �������������� ������ ExtHeap. �� ����������� ��������� ������ capacity. size = %zd, elemSize = %zd.",
				  size, elemSize);
		ExtHeapDump(extHeap);

		ProgramStatus status = ExtHeapResize(extHeap);
		if (status == ProgramStatus::Fault)
		{
			TRACE_ERROR();
			return nullptr;
		}

		// ���������, ��� ��������� ����� ������ ��������� �������� ����������� �������.
		// ����� ������������� ������������, ��� ����������� ��������� �������� �� ����������.
		if (size + elemSize > extHeap->Capacity)
		{
			LOG_F_WRN("�� ����������� ����������� ��������� ������ � ExtHeap. size = %zd, elemSize = %zd.",
					  size, elemSize);
			ExtHeapDump(extHeap);
		}
	}

	extHeap->Size += elemSize;

	return extHeap->Heap + size;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

static ProgramStatus ExtHeapResize(ExtHeap* const extHeap)
{
	assert(extHeap);
	assert(extHeap->Capacity > 0);
	assert(extHeap->Resize   > 1);

	size_t newCapacity = (size_t)((float)extHeap->Capacity * extHeap->Resize);

	char* resized = (char*)realloc(extHeap->Heap, newCapacity);
	if (!resized)
	{
		LOG_ERR_MEMORY(newCapacity);
		return ProgramStatus::Fault;
	}

	// ���� ��������� ������ ��������� � �������, �� ��������� ��������� �� ��������.
	extHeap->Heap     = resized;
	extHeap->Capacity = newCapacity;
	extHeap->ReallocCounter++;

	return ProgramStatus::Ok;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#undef ExtHeapDump
void ExtHeapDump(const ExtHeap* const extHeap,
				 const char* const funct, const char* const file, const size_t line)
{
	assert(extHeap);
	assert(funct);
	assert(file);

	LOG_F_DBG("��������� ExtHeap [%p]:\n"
			  "Heap           = %p;\n"
			  "Size           = %zd;\n"
			  "Capacity       = %zd;\n"
			  "Resize         = %f;\n"
			  "ReallocCounter = %zd.\n"
			  "������� ExtHeapDump ������� %s �� ����� %s � ������ %zd.",
			  extHeap, extHeap->Heap, extHeap->Size, extHeap->Capacity, extHeap->Resize,
			  extHeap->ReallocCounter, funct, file, line);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///