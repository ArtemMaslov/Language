///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Файл грамматики языка. 
// 
// Описание основных типов данных и функций.
//
// Версия: 1.0.1.0
// Дата последнего изменения: 12:15 29.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef LANGUAGE_GRAMMAR_H
#define LANGUAGE_GRAMMAR_H

#include <stddef.h>

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define GRAMMAR(type, grammar, keyword, ...) \
	keyword,

/// Тип ключевого слова.
enum class KeywordType
{
	Null = -1, ///< Лексема не является ключевым словом.
#include "keywords.inc"
};

#undef GRAMMAR

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define GRAMMAR(type, grammar, oper, ...) \
	oper,

/// Тип оператора.
enum class OperatorType
{
	Null = -1, ///< Лексема не является оператором.
#include "operators.inc"
};

#undef GRAMMAR

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define GRAMMAR(type, grammar, specSymbol, ...) \
	specSymbol,

/// Тип специального символа.
enum class SpecialSymbolType
{
	Null = -1, ///< Лексема не является специальным символом.
#include "special_symbols.inc"
};

#undef GRAMMAR

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

// !Название LngTokenType, а не TokenType, потому что TokenType - это одно из значений enum подключаемой библиотеки.

/// Тип лексемы.
enum class LngTokenType
{
	/// Ключевое слово.
	Keyword       = 0,
	/// Оператор.
	Operator      = 1,
	/// Идентификатор.
	Identifier    = 2,
	/// Специальный символ.
	SpecialSymbol = 3,
	/// Число.
	Number        = 4
};

/// Тип грамматики.
enum class GrammarType
{
	Universal = 1 << 0, ///< Универсальная грамматика.
	Russian   = 1 << 1, ///< Русская грамматика.
	English   = 1 << 2, ///< Английская грамматика.
};

/// Структура для описания лексемы.
struct GrammarToken
{
	/// Класс лексемы.
	LngTokenType Type;
	/// Тип грамматики, в котором определена лексема.
	GrammarType  Grammar;

	/// Тип лексемы.
	union
	{
		/// Ключевое слово.
		KeywordType       Keyword;
		/// Оператор.
		OperatorType      Operator;
		/// Специальный символ.
		SpecialSymbolType SpecialSymbol;
	} Value;

	/// Символьное представление лексемы.
	const char*  Name;
	/// Длина символьного представления.
	const size_t NameSize;
};

/// Таблица лексем операторов.
extern GrammarToken Operators[];
/// Количество лексем операторов.
extern const size_t OperatorsSize;

/// Таблица лексем специальных символов.
extern GrammarToken SpecialSymbols[];
/// Количество лексем специальных символов.
extern const size_t SpecialSymbolsSize;

/// Таблица лексем ключевых слов.
extern GrammarToken Keywords[];
/// Количество лексем ключевых слов.
extern const size_t KeywordsSize;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Получить описание лексемы по её типу.
 * 
 * @warning В Release нет проверки на правильность передаваемого типа лексемы.
 * 
 * @param keyword Тип ключевого слова.
 * 
 * @return Указатель на ключевое слово.
*/
const GrammarToken* GrammarGetToken(const KeywordType keyword);

/**
* @brief  Получить описание лексемы по её типу.
* 
* @warning В Release нет проверки на правильность передаваемого типа лексемы.
*
* @param oper Тип оператора.
* 
* @return Указатель на оператор.
*/
const GrammarToken* GrammarGetToken(const OperatorType oper);

/**
* @brief  Получить описание лексемы по её типу.
* 
* @warning В Release нет проверки на правильность передаваемого типа лексемы.
*
* @param specSym Тип специального символа.
* 
* @return Указатель на специальный символ.
*/
const GrammarToken* GrammarGetToken(const SpecialSymbolType specSym);

/**
 * @brief  Получить строку для оператора для GraphViz.
 * 
 * @param oper Тип оператора.
 * 
 * @return Указатель на строку.
 */
const char* const GrammarGetGraphVizName(const OperatorType oper);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !LANGUAGE_GRAMMAR_H