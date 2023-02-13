///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль работы с метками в ассемблерном представлении.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 19:00 08.02.2023
//
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <string.h>

#include "../../Modules/Logs/Logs.h"

#include "asm_defs.h"
#include "asm_labels.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

ProgramStatus LabelTableConstructor(AsmLabelTable *const table)
{
    assert(table);

    ProgramStatus status = ProgramStatus::Ok;

    status = ExtArrayConstructor(&table->Labels, sizeof(AsmLabelNode), AsmLabelsDefaultCount);
    if (status != ProgramStatus::Ok)
    {
        TRACE_ERROR();
        return status;
    }

    status = ExtArrayConstructor(&table->Inserts, sizeof(AsmLabelInsert), AsmInsertsDefaultCount);
    if (status != ProgramStatus::Ok)
    {
        ExtArrayDestructor(&table->Labels);
        TRACE_ERROR();
        return status;
    }

    return ProgramStatus::Ok;
}

ProgramStatus LabelTableAddLabel(AsmLabelTable *const table, const char *const labelName, const size_t offset)
{
    assert(table);
    assert(labelName);

    AsmLabelNode label   = {};
    label.LabelName      = labelName;
    label.RelativeOffset = offset;

    ProgramStatus status = ExtArrayAddElem(&table->Labels, &label);
    if (status != ProgramStatus::Ok)
    {
        TRACE_ERROR();
        return status;
    }
    
    return ProgramStatus::Ok;
}

ProgramStatus LabelTableAddInsert(AsmLabelTable *const table, const char *const labelName, int *const address)
{
    assert(table);
    assert(labelName);
    assert(address);

    AsmLabelInsert insert = {};
    insert.Insert         = address;
    insert.LabelName      = labelName;

    ProgramStatus status = ExtArrayAddElem(&table->Inserts, &insert);
    if (status != ProgramStatus::Ok)
    {
        TRACE_ERROR();
        return status;
    }

    return ProgramStatus::Ok;
}

void LabelTabelApplyOffset(const AsmLabelTable *const table, const size_t offset)
{
    assert(table);

    const ExtArray *const labels = &table->Labels;
    const size_t labelsCount = table->Labels.Size;

    for (size_t st = 0; st < labelsCount; st++)
    {
        AsmLabelNode *label = (AsmLabelNode *)ExtArrayGetElemAt(labels, st);

        label->RelativeOffset += offset;
    }
}

void LabelTableWrite(const AsmLabelTable *const table)
{
    assert(table);

    const ExtArray *const labels = &table->Labels;
    const ExtArray *const inserts = &table->Inserts;

    const size_t insertCount = table->Inserts.Size;
    const size_t labelsCount = table->Labels.Size;

    for (size_t st = 0; st < insertCount; st++)
    {
        AsmLabelInsert *insert = (AsmLabelInsert *)ExtArrayGetElemAt(inserts, st);

        for (size_t st1 = 0; st1 < labelsCount; st1++)
        {
            AsmLabelNode *label = (AsmLabelNode *)ExtArrayGetElemAt(labels, st1);

            if (strcmp(label->LabelName, insert->LabelName) == 0)
            {
                *insert->Insert = label->RelativeOffset - (*insert->Insert);
                break;
            }
        }
    }
}

void LabelTableDestructor(AsmLabelTable *const table)
{
    assert(table);

    ExtArrayDestructor(&table->Labels);
    ExtArrayDestructor(&table->Inserts);
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///