///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль работы с метками в ассемблерном представлении.
//
// Версия: 1.0.0.0
// Дата последнего изменения: 19:00 08.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef ASM_LABELS_H
#define ASM_LABELS_H

/// Таблица меток.
struct AsmLabelTable
{
    /**
     * @brief Массив меток.
     * ExtArray<AsmLabelNode>
    */
	ExtArray Labels;
    /**
     * @brief Массив вставок.
     * ExtArray<AsmLabelInsert>
    */
	ExtArray Inserts;
};

/// Структура метки.
struct AsmLabelNode
{
    /// Имя метки.
	const char* LabelName;
    /// Относительное смещение в коде программы.
	size_t      RelativeOffset;
};

/// Структура вставки.
struct AsmLabelInsert
{
    /// Имя метки.
	const char* LabelName;
    /// Адрес вставки в коде программы.
	int* Insert;
};

/// Начальное количество меток таблицы.
const size_t AsmLabelsDefaultCount  = 1024;
/// Начальное количество вставок таблицы.
const size_t AsmInsertsDefaultCount = 256;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Конструктор таблицы меток.
 * 
 * @param table Таблица меток.
 * 
 * @return ProgramStatus::ExtArray.
 *         ProgramStatus::NoErrors.
 */
ProgramStatus LabelTableConstructor(AsmLabelTable* const table);

/**
 * @brief  Добавить метку в таблицу.
 * 
 * @param table     Таблица меток.
 * @param labelName Имя метки.
 * @param offset    Относительное смещение.
 * 
 * @return ProgramStatus::ExtArrayAddElem.
 *         ProgramStatus::NoErrors.
 */
ProgramStatus LabelTableAddLabel(AsmLabelTable* const table, const char* const labelName, const size_t offset);

/**
 * @brief  Добавить вставку.
 * 
 * @param table     Таблица меток.
 * @param labelName Имя метки.
 * @param address   Адрес вставки.
 * 
 * @return ProgramStatus::ExtArrayAddElem.
 *         ProgramStatus::NoErrors.
 */
ProgramStatus LabelTableAddInsert(AsmLabelTable* const table, const char* const labelName, int* const address);

/**
 * @brief  Перевести относительное смещение между командами в абсолютное.
 * 
 * @param table  Таблица меток.
 * @param offset Смещение, добавляемое к относительному, для получения абсолютного.
 */
void LabelTabelApplyOffset(const AsmLabelTable* const table, const size_t offset);

/**
 * @brief  Выполнить записи смещений по всем вставкам.
 * 
 * @param table Таблица меток.
 */
void LabelTableWrite(const AsmLabelTable* const table);

/**
 * @brief  Деструктор таблицы меток.
 * 
 * @param table Таблица меток.
 */
void LabelTableDestructor(AsmLabelTable* const table);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !ASM_LABELS_H