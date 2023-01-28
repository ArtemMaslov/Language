#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <Windows.h>

#include "List.h"
#include "List_Config.h"
#include "List_Logs.h"
#include "_list_private.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

/// Структура для проверки связности списка.
/// 0, если данный элемент ещё не появлялся.
/// 1, если данный элемент уже появился. Это будет ошибкой.
struct ListNodeCheck
{
    /// Проверка массива Next.
    char Next;
    /// Проверка массива Prev.
    char Prev;
};

int ListVerify(List* list)
{
    TRACE_VERIFY();
    assert(list);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    list->Status = LIST_ERR_NO_ERRORS;

    const size_t capacity = list->Capacity;
    const size_t size     = list->Size;

    ListNode* nodes       = list->Nodes;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    if ((size >  0 && list->Head == 0) ||
        (size == 0 && list->Head != 0) ||
        (list->Head != nodes[0].Next))
        list->Status |= LIST_ERR_HEAD;

    if ((size == 0 && list->Tail != 0) ||
        (size >  0 && list->Tail == 0) ||
        (list->Tail != nodes[0].Prev))
        list->Status |= LIST_ERR_TAIL;

    if (size > capacity)
        list->Status |= LIST_ERR_SIZE;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    size_t fillCount = 0;
    size_t prevCount = 0;
    size_t nextCount = 0;

    size_t nextIndex = 0;
    size_t prevIndex = 0;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    for (size_t st = 1; st <= capacity; st++)
    {
        if (!CHECK_FREE_ELEM(list, st))
            fillCount++;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    ListNodeCheck* nodeChecker = (ListNodeCheck*)calloc(list->Capacity + 1, sizeof(ListNodeCheck));

    if (nodeChecker)
    {
        for (size_t st = 0; st <= size + 1; st++)
        {
            if (!CHECK_FREE_ELEM(list, nextIndex))
            {
                nextIndex = nodes[st].Next;

                // Массив Next имеет малые закольцованные части, так быть не должно. 
                if (nodeChecker[nextIndex].Next != 0)
                {
                    list->Status |= LIST_ERR_DATA;
                    break;
                }

                nodeChecker[nextIndex].Next++;
                nextCount++;
            }

            if (!CHECK_FREE_ELEM(list, prevIndex))
            {
                prevIndex = nodes[st].Prev;

                // Массив Prev имеет малые закольцованные части, так быть не должно. 
                if (nodeChecker[nextIndex].Prev++ != 0)
                {
                    list->Status |= LIST_ERR_DATA;
                    break;
                }

                prevCount++;
            }
        }

        free(nodeChecker);
    }
    else
    {
        list->Status |= LIST_ERR_MEMORY;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    if (nextIndex != 0              || // Массив Next не закольцован.
        prevIndex != 0              || // Массив Prev не закольцован.
        fillCount != size           || // Количество элементов не соответствует значению Size.
        nextCount != size           || // + 0 так как считаем не количество связей, 
                                       // которых Size + 1, а количество элементов.
        prevCount != size)             // + 0 <---^
        list->Status |= LIST_ERR_DATA;

    return list->Status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

void ListDump(List* list, size_t dataStartIndex, size_t dataEndIndex)
{
    TRACE_VERIFY();
    assert(list);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    size_t capacity = list->Capacity;
    size_t size     = list->Size;

    size_t filled = 0;

    const size_t bufferSize = 1024;

    char buffer[bufferSize] = "";

    FILE* log = LogBeginDump(LOG_SIG_GENERAL, LOG_LVL_DEBUG);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    sprintf(buffer,
            "List<%p> dump.\n\n"
            "\tNodes    = <%p>\n"
            "\tFree     = %zd\n"
            "\tHead     = %zd\n"
            "\tTail     = %zd\n"
            "\tSize     = %zd\n"
            "\tCapacity = %zd\n"
            "\tSorted   = %s\n"
            "\tStatus   = %d\n\n"
            "Data info:\n\n",
            (void*)list,
            (void*)list->Nodes,
            list->Free,
            list->Head,
            list->Tail,
            list->Size,
            list->Capacity,
            (list->Sorted)? "true" : "false",
            list->Status);

    fputs(buffer, log);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    const size_t endIndex = list->Size < dataEndIndex? list->Size : dataEndIndex;

    const ListNode* nodes = list->Nodes;

    for (size_t st = dataStartIndex; st < dataEndIndex; st++)
    {
        sprintf(buffer,
                "\tIndex %zd\n\t\tn = %zd\n\t\tp = %zd\n\t\td = ",
                st,
                nodes[st].Next,
                nodes[st].Prev);

        fputs(buffer, log);

        ListElementToString(buffer, bufferSize, &nodes[st].Data);

        fputs(buffer, log);

        fputs("\n\n", log);
    }

    fputs("List dump end.\n", log);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    LogEndDump(LOG_SIG_GENERAL);
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const char* GraphHeaderBegin = 
"digraph G\n"
"{\n"
    "\tgraph[dpi = 100];\n"
    "\tsplines   = ortho;\n"
    "\tranksep   = 1;\n"
    "\tbgcolor   = \"#303030\";\n\n"
 
    "\tedge\n"
    "\t[\n"
        "\t\tminlen   = 3,\n"
        "\t\tpenwidth = 3,\n"
        "\t\tcolor    = \"green\"\n"
    "\t];\n\n"
 
    "\tnode\n"
    "\t[\n"
        "\t\tshape     = \"record\",\n"
        "\t\tstyle     = \"rounded, filled\",\n"
        "\t\tfillcolor = \"#404040\",\n"
        "\t\tfontsize  = 18,\n"
        "\t\tpenwidth  = 1,\n"
        "\t\tcolor     = \"#F9D423\",\n"
        "\t\tmargin    = \"0.05 0.4\",\n"
        "\t\tfontcolor = \"#E1F5C4\"\n"
    "\t]\n\n"

    "\t{\n"
    "\t\trankdir       = LR;\n"
    "\t\trank          = min;\n"
    "\t\tminNode[label = \"utilNode\", style = invis];\n";

const char* GraphHeaderNodeInt = "\t\t%s[label    = \"%s \\n %zd\"];\n";
const char* GraphHeaderNodeStr = "\t\t%s[label    = \"%s \\n %s\"];\n";

const char* GraphHeaderEnd =
    "\t}\n\n";

const char* GraphListDataBegin =
    "\t{\n"
        "\t\trankdir = LR;\n"
        "\t\trank    = same;\n\n";

const char* GraphListDataEnd = 
    "\t}\n\n"

    "\t{\n"
        "\t\trank = max;\n"
        "\t\tmaxNode[label = \"utilNode\", style = invis];\n"
    "\t}\n\n"
"\tminNode -> f0[style = invis]\n"
"\tmaxNode -> f0[style = invis]\n\n";

const char* PrevEdgeColor        = "\tedge [color = \"#4ECDC4\", constraint = false];\n\n";
const char* NextEdgeColor        = "\tedge [color = \"#C7F464\", constraint = false];\n\n";
const char* ListHeaderEdgeColor  = "\tedge [color = \"#FF3E50\", constraint = false];\n\n";
const char* ListConstraint       = "\tedge [style = invis, constraint = true];\n\n";

const char* EdgeFormatIntInt = "\tf%zd -> f%zd;\n";
const char* EdgeFormatStrInt = "\t%s -> f%zd;\n";
const char* EdgeFormatIntStr = "\tf%zd -> %s;\n";

const char* NodeBegin = "\t\tf%zd [label = \"{{ <l3> index \\n %zd }|{ value \\n";
const char* NodeEnd   = " }|{ <l1> prev \\n %zd | <l2> next \\n %zd }}\"];\n";

/// Тип выходного файла, который создаст GraphViz. png или svg.
#define LIST_GRAPH_TYPE "png"

int ListGraphicDump(List* list)
{
    TRACE_VERIFY();
    assert(list);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    const  size_t filePathSize = 100;
    static size_t imageIndex   = 0;

    const char* folder          = LOGS_FOLDER LIST_LOG_IMAGE_FOLDER;

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    // Создаём папку, куда будут сохраняться изображения, чтобы не мусорить папку с логами.
    if (!DirectoryExist(folder) &&
        !CreateDirectoryA(folder, nullptr))
    {
        LOG_F_ERR("Ошибка создания папки \"%s\"", folder);

        list->Status |= LIST_ERR_FOLDER;

        return list->Status;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    // Создаём файл описания графа .gv для создания изображения с помощью GraphViz.
    char convertBuffer[ListToStringSize] = "";
    
    const char* fileGraphvizPath            = LOGS_FOLDER LIST_LOG_IMAGE_FOLDER "listDump.gv";
    char        fileImagePath[filePathSize] = "";

    snprintf(fileImagePath, filePathSize, LOGS_FOLDER LIST_LOG_IMAGE_FOLDER "listDump%zd." LIST_GRAPH_TYPE, imageIndex);

    FILE* outGraphFile = fopen(fileGraphvizPath, "w");

    if (!outGraphFile)
    {
        LOG_F_ERR("Ошибка создания файла \"%s\"", fileGraphvizPath);

        list->Status |= LIST_ERR_FILE;

        return list->Status;
    }

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    // Создание заголовка графа.
    fputs(GraphHeaderBegin, outGraphFile);

    fprintf(outGraphFile, GraphHeaderNodeInt, "Head",     "Head",     list->Head);
    fprintf(outGraphFile, GraphHeaderNodeInt, "Tail",     "Tail",     list->Tail);
    fprintf(outGraphFile, GraphHeaderNodeInt, "Free",     "Free",     list->Free);
    fprintf(outGraphFile, GraphHeaderNodeInt, "Size",     "Size",     list->Size);
    fprintf(outGraphFile, GraphHeaderNodeInt, "Capacity", "Capacity", list->Capacity);
    fprintf(outGraphFile, GraphHeaderNodeInt, "Status",   "Status",   list->Status);
    fprintf(outGraphFile, GraphHeaderNodeStr, "Sorted",   "Sorted",   list->Sorted ? "true" : "false");

    fputs(GraphHeaderEnd, outGraphFile);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\

    // Создание узлов элементов списка.
    fputs(GraphListDataBegin, outGraphFile);

    for (size_t st = 0; st <= list->Capacity; st++)
    {
        fprintf(outGraphFile, NodeBegin, st, st);

        ListElementToString(convertBuffer, ListToStringSize, &list->Nodes[st].Data);

        fputs(convertBuffer, outGraphFile);

        fprintf(outGraphFile, NodeEnd, list->Nodes[st].Prev, list->Nodes[st].Next);
    }

    fputs(GraphListDataEnd, outGraphFile);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    // Создание связи Prev.
    fputs(PrevEdgeColor, outGraphFile);
    for (size_t st = 1; st <= list->Capacity; st++)
        if (list->Nodes[st].Prev != 0)
            fprintf(outGraphFile, EdgeFormatIntInt, st, list->Nodes[st].Prev);

    fputc('\n', outGraphFile);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    // Создание связи Next.
    fputs(NextEdgeColor, outGraphFile);
    for (size_t st = 1; st <= list->Capacity; st++)
        if (list->Nodes[st].Next != 0)
            fprintf(outGraphFile, EdgeFormatIntInt, st, list->Nodes[st].Next);

    fputc('\n', outGraphFile);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
    
    // Создание линий связи информационных полей списка с узлами элементов.
    fputs(ListHeaderEdgeColor, outGraphFile);

    fprintf(outGraphFile, EdgeFormatStrInt, "Head", list->Head);
    fprintf(outGraphFile, EdgeFormatIntStr, list->Tail, "Tail");
    fprintf(outGraphFile, EdgeFormatStrInt, "Free", list->Free);

    fputc('\n', outGraphFile);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
   
    // Создание вспомогательных линий, чтобы узлы располагались в правильном порядке.
    fputs(ListConstraint, outGraphFile);

    for (size_t st = 1; st <= list->Capacity; st++)
        fprintf(outGraphFile, EdgeFormatIntInt, st - 1, st);

    fputs("\n}", outGraphFile);

    //***\\---//***\\-----//***\\---//***\\-----//*****\\-----//***\\---//***\\-----//***\\---//***\\
   
    // Закрытие файла, преобразование его в изображение.
    fclose(outGraphFile);

    char cmd[256] = "";
    sprintf(cmd, "dot \"%s\" -T" LIST_GRAPH_TYPE " > \"%s\"", fileGraphvizPath, fileImagePath);
    int status = system(cmd);

    // Если нет ошибок, то добавляем изображение в лог.
    if (status == 0)
    {
        snprintf(fileImagePath, filePathSize, LIST_LOG_IMAGE_FOLDER "listDump%zd." LIST_GRAPH_TYPE, imageIndex);

        LogAddImage(LOG_SIG_GENERAL, fileImagePath);
    }

    imageIndex++;

    return list->Status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 