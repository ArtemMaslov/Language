#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"
#include "List_Config.h"
#include "List_Logs.h"
#include "_list_private.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static int ListResize(List* list);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

int ListConstructor(List* list, const size_t capacity)
{
    LOG_LIST_TRACE_CTOR;

    assert(list);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    list->Capacity = capacity;
    list->Sorted   = true;
    list->Status   = LIST_ERR_NO_ERRORS;

    list->Head = 0;
    list->Tail = 0;
    list->Size = 0;
    list->Free = 1;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    list->Nodes = (ListNode*)calloc(capacity + 1, sizeof(ListNode));

    if (!list->Nodes)
    {
        LOG_LIST_ERR_MEMORY;

        list->Status |= LIST_ERR_MEMORY;

        return list->Status;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    ListNode* nodes = list->Nodes;

    for (size_t st = 1; st < capacity; st++)
        nodes[st].Next = st + 1;

    nodes[capacity].Next = 0;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    return list->Status;
}

void ListDestructor(List* list)
{
    LOG_LIST_TRACE_CTOR;

    assert(list);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    free(list->Nodes);

    memset(list, 0, sizeof(List));
}

void ClearList(List* list)
{
    LOG_LIST_TRACE_CTOR;

    assert(list);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    const size_t capacity = list->Capacity;

    list->Sorted = true;

    list->Head = 0;
    list->Tail = 0;
    list->Size = 0;
    list->Free = 1;

    ListNode* nodes = list->Nodes;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    // Обнуляем массивы
    memset(nodes, 0, (capacity + 1) * sizeof(ListType));

    // Создаём список пустых элементов.
    for (int st = 0; st <= capacity; st++)
        nodes[st].Next = st + 1;

    // Закольцовываем массив.
    nodes[capacity].Next = 0;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

int ListAddElemAfter(List* list, const ListType* value, const size_t dataIndex)
{
    LOG_LIST_TRACE_FUNC_0;

    assert(list);
    assert(value);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    if (list->Size > list->Capacity)
    {
        LOG_F_LIST_ERR("Размер списка = %zd больше вместимости = %zd", list->Size, list->Capacity);

        list->Status |= LIST_ERR_SIZE;

        return list->Status;
    }

    if (CHECK_FREE_ELEM(list, dataIndex))
    {
        LOG_F_LIST_ERR("Попытка добавить элемент после пустого. dataIndex = %zd.", dataIndex);

        list->Status |= LIST_ERR_ADD_FREE;

        return list->Status;
    }

    // Список полон, выделяем место.
    if (CHECK_LIST_FULL(list))
    {
        if (ListResize(list) != LIST_ERR_NO_ERRORS)
            return list->Status;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    ListNode* nodes = list->Nodes;

    // Найти пустое место в списке.
    const size_t insertElemIndex = FIND_EMPTY_PLACE(list);

    // Если вставляем элемент не в конец списка, то сбивается сортировка.
    // Иначе он становится хвостом списка.
    if (list->Tail != dataIndex) 
        list->Sorted = false;
    else
        list->Tail = insertElemIndex;

    // Если вставляем элемент перед головой списка.
    if (dataIndex == 0)
        list->Head = insertElemIndex;

    // Копируем содержимое вставляемого элемента.
    memcpy(&nodes[insertElemIndex].Data, value, sizeof(ListType)); 
    
    // Поле Next пустых элементов является односвязным списком пустых мест.
    list->Free = nodes[insertElemIndex].Next;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    // Изменяем поле Next в списке:

    // Поле Next вставляемого элемента.
    nodes[insertElemIndex].Next = nodes[dataIndex].Next;
    // Поле Next предшествующего элемента.
    nodes[dataIndex].Next = insertElemIndex;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    // Изменяем поле Prev:

    // Поле Prev вставляемого элемента.
    nodes[insertElemIndex].Prev = dataIndex;
    // Поле Prev следующего за вставляемым элементом.
    nodes[nodes[insertElemIndex].Next].Prev = insertElemIndex;

    // Размер списка увеличился.
    list->Size++;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    return list->Status;
}

int ListAddElemBefore(List* list, const ListType* value, const size_t dataIndex)
{
    LOG_LIST_TRACE_FUNC_0;

    assert(list);
    assert(value);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    if (list->Size > list->Capacity)
    {
        LOG_F_LIST_ERR("Размер списка = %zd больше вместимости = %zd", list->Size, list->Capacity);

        list->Status |= LIST_ERR_SIZE;

        return list->Status;
    }

    if (CHECK_FREE_ELEM(list, dataIndex))
    {
        LOG_F_LIST_ERR("Попытка добавить элемент перед пустым. dataIndex = %zd.", dataIndex);

        list->Status |= LIST_ERR_ADD_FREE;

        return list->Status;
    }

    // Список полон, выделяем место.
    if (CHECK_LIST_FULL(list))
    {
        if (ListResize(list) != LIST_ERR_NO_ERRORS)
            return list->Status;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    ListNode* nodes = list->Nodes;

    // Найти пустое место в списке.
    const size_t insertElemIndex = FIND_EMPTY_PLACE(list);

    // В общем случае любая вставка в начало списка сбивает сортировку
    list->Sorted = false; 

    // Если вставляем элемент перед головой списка, то он становиться новой головой.
    if (list->Head == dataIndex)
        list->Head = insertElemIndex;

    // Копируем содержимое вставляемого элемента.
    memcpy(&nodes[insertElemIndex].Data, value, sizeof(ListType));

    // Поле Next пустых элементов является односвязным списком пустых мест.
    list->Free = nodes[insertElemIndex].Next;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    // Изменяем поле Prev:

    // Поле Prev вставляемого элемента.
    nodes[insertElemIndex].Prev = nodes[dataIndex].Prev;
    // Поле Prev следующего за вставляемым элементом.
    nodes[dataIndex].Prev = insertElemIndex;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    // Изменяем поле Next в списке:

    // Поле Next предшествующего элемента.
    nodes[nodes[insertElemIndex].Prev].Next = insertElemIndex;
    // Поле Next вставляемого элемента.
    nodes[insertElemIndex].Next = dataIndex;

    // Размер списка увеличился.
    list->Size++;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    return list->Status;
}

int ListRemoveElem(List* list, const size_t dataIndex)
{
    LOG_LIST_TRACE_FUNC_0;

    assert(list);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    if (list->Size <= 0)
    {
        LOG_LIST_ERR("Попытка удалить элемент из пустого списка.");

        list->Status |= LIST_ERR_REMOVE_EMPTY;

        return list->Status;
    }

    if (CHECK_FREE_ELEM(list, dataIndex))
    {
        LOG_F_LIST_ERR("Попытка удалить пустой элемент. dataIndex = %zd.", dataIndex);

        list->Status |= LIST_ERR_REMOVE_FREE;

        return list->Status;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    ListNode* nodes = list->Nodes;

    size_t nextIndex = nodes[dataIndex].Next;
    size_t prevIndex = nodes[dataIndex].Prev;

    // Удаление элемента из середины сбивает сортировку.
    if (dataIndex != list->Tail && dataIndex != list->Head)
        list->Sorted = false;

    // Удалить элемент списка.
    memset(nodes + dataIndex, 0, sizeof(ListNode));

    // Изменяем связи элементов списка
    nodes[nextIndex].Prev = prevIndex;
    nodes[prevIndex].Next = nextIndex;

    // Изменяем голову и хвост списка.
    if (list->Tail == dataIndex)
        list->Tail = prevIndex;

    if (list->Head == dataIndex)
        list->Head = nextIndex;

    // Помечаем освободившийся элемент пустым.
    nodes[dataIndex].Next = list->Free;
    nodes[dataIndex].Prev = 0;

    list->Free = dataIndex;

    // Уменьшаем размер списка.
    list->Size--;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    return list->Status;
}

ListType* ListGetElemAt(List* list, const size_t logicalIndex)
{
    LOG_LIST_TRACE_FUNC_0;

    assert(list);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    if (logicalIndex > list->Size)
    {
        LOG_F_LIST_ERR("Попытка получить доступ к не существующему элементу списка. "
                     "logicalIndex = %zd, list->Size = %zd", logicalIndex, list->Size);

        list->Status |= LIST_ERR_SELECT_FREE;

        return nullptr;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    ListNode* nodes = list->Nodes;

    // Если список отсортирован, то логический индекс равен физическому.
    // Иначе ищем индекс за линейное время.
    if (list->Sorted)
    {
        return &nodes[logicalIndex + list->Head].Data;
    }
    else
    {
        size_t cur   = list->Head;
        size_t end   = list->Tail;
        size_t index = 0;

        while (cur != end)
        {
            if (index == logicalIndex)
                return &nodes[cur].Data;

            index++;
            cur = nodes[cur].Next;
        }
    }

    // Данный код физически не может исполниться.
    LOG_LIST_ERR("Непредвиденная ошибка. Исполнение не возможного кода. "
                 "Логический элемент не был найден в списке.");

    list->Status |= LIST_ERR_SELECT_FREE;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    return nullptr;
}

size_t GetPhysicalIndex(List* list, const size_t logicalIndex)
{
    LOG_LIST_TRACE_FUNC_0;

    assert(list);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    if (logicalIndex > list->Size)
    {
        LOG_F_LIST_ERR("Попытка получить доступ к не существующему элементу списка. "
                       "logicalIndex = %zd, list->Size = %zd", logicalIndex, list->Size);

        list->Status |= LIST_ERR_SELECT_FREE;

        return 0;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    // Если список отсортирован, то логический индекс совпадает с физическим.
    if (list->Sorted)
        return logicalIndex + list->Head;

    // Иначе поиск за линейное время.
    size_t cur = list->Head;

    for (size_t st = 0; st < logicalIndex; st++)
        cur = list->Nodes[cur].Next;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    return cur;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

int ListSort(List* list)
{
    LOG_LIST_TRACE_FUNC_0;

    assert(list);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    const size_t listSize     = list->Size;
    const size_t listCapacity = list->Capacity;
    size_t       currentIndex = 0;
    size_t       nextIndex    = 0;

    ListNode     tmp          = {};

    ListNode*    nodes        = list->Nodes;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    // Размещаем узлы в нужном порядке.
    for (size_t correctIndex = 0; correctIndex <= listSize; correctIndex++)
    {
        nextIndex = nodes[currentIndex].Next;

        // Если элемент находится не на своём месте, то его нужно поставить на правильное место.
        if (currentIndex != correctIndex)
        {
            memcpy(&tmp, nodes + currentIndex, sizeof(ListNode));

            memcpy(nodes + currentIndex, nodes + correctIndex, sizeof(ListNode));

            memcpy(nodes + correctIndex, &tmp, sizeof(ListNode));
        }

        currentIndex = nextIndex;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    // Устанавливаем голову и хвост.
    list->Head = 1;
    list->Tail = listSize;

    // Устанавливаем значения указателей Next и Prev.

    // Устанавливаем значения Next и Prev для заполненной части.
    for (size_t listIndex = 0; listIndex <= listSize; listIndex++)
    {
        nodes[listIndex].Next = listIndex + 1;
        nodes[listIndex].Prev = listIndex - 1;
    }

    // Закольцовываем массивы.
    nodes[0].Prev        = listSize;
    nodes[listSize].Next = 0;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    // Инициализируем свободные элементы.
    if (listSize < listCapacity)
    {
        // Свободные элементы есть.
        list->Free = listSize + 1;

        // Очищаем новую память.
        memset(nodes + (listSize + 1), 0, (listCapacity - listSize) * sizeof(ListNode));

        // Устанавливаем значения Next и Prev для свободных элементов.
        for (size_t st = listSize + 1; st < listCapacity; st++)
            nodes[st].Next = st + 1;

        // Последний элемент указывает на 0.
        nodes[listCapacity].Next = 0;
    }
    else
        list->Free = 0;            // Список полон.

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    return list->Status;
}

size_t ListFind(const List* list, const ListType* element)
{
    LOG_LIST_TRACE_FUNC_0;

    assert(list);
    assert(element);

    LIST_ASSERT_STATUS;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    size_t          current   = list->Head;
    const size_t    listSize  = list->Size;
    const ListNode* nodes     = list->Nodes;
    const ListType  elemValue = *element;

    // Линейный поиск элементов.
    for (size_t st = 0; st < listSize; st++)
    {
        if (ListComparator(elemValue, nodes[current].Data))
            return current;

        current = nodes[current].Next;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    return 0;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static int ListResize(List* list)
{
    LOG_LIST_TRACE_FUNC_1;

    assert(list);

    LIST_ASSERT_STATUS;
    
    // Перед вызовом функции нужно проверять, что список полон.
    assert(CHECK_LIST_FULL(list));

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    size_t oldCapacity = list->Capacity;
    size_t newCapacity = (size_t)((double)oldCapacity * ListResizeCoef);

    if (newCapacity <= oldCapacity)
    {
        LOG_LIST_ERR_MEMORY;

        list->Status |= LIST_ERR_MEMORY;

        return list->Status;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    ListNode* nodes = (ListNode*)realloc(list->Nodes, (newCapacity + 1) * sizeof(ListNode));

    if (!nodes)
    {
        LOG_LIST_ERR_MEMORY;

        list->Status |= LIST_ERR_MEMORY;

        return list->Status;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    // Выделение памяти произошло успешно, сохраняем указатель на новый блок памяти.
    list->Nodes = nodes;
    
    // Очищаем новую память.
    memset(nodes + (oldCapacity + 1), 0, (newCapacity - oldCapacity) * sizeof(ListNode));

    // Добавляем выделенную память в список свободных элементов.
    for (size_t st = oldCapacity + 1; st <= newCapacity; st++)
        nodes[st].Next = st + 1;

    // Ново выделенная память добавляется перед старой свободной памятью, от этого собьётся сортировка.
    nodes[newCapacity].Next = 0;
    list->Free = oldCapacity + 1;

    list->Capacity = newCapacity;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    return list->Status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 