///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
//  
// Версия: 1.0.1.0
// Дата последнего изменения: 14:44 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include "../LanguageGrammar/LanguageGrammar.h"
#include "../Modules/ExtArray/ExtArray.h"
#include "../Modules/ExtHeap/ExtHeap.h"
#include "../Front end//Lexer/Identifier.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

struct FunctCallNode;
struct FunctParamNode;
struct FunctDefNode;
struct BinaryOperatorNode;
struct UnaryOperatorNode;
struct ExpressionNode;
struct VariableNode;
struct IfNode;
struct WhileNode;
struct InstructionNode;
struct ConstructionNode;
struct InputNode;
struct OutputNode;

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#define AST_NODE(type) type,
/// Тип узла АСД.
enum class AstNodeType
{
	Variable = 0,
	GlobVar  = 0,
#include "ast_nodes.inc"
};
#undef AST_NODE

/// Структура абстрактного синтаксического дерева АСД.
struct AST
{
	/**
	 * @brief Массив строительных узлов.
	 * ExtArray<ConstructionNode>
	*/
	ExtArray ConstrNodes;
	
	/**
	 * @brief Куча узлов АСД.
	*/
	ExtHeap Nodes;

	/// Таблица идентификаторов.
	IdentifierTable* Identifiers;
};

/// Начальный размер узлов АСД в ExtHeap.
const size_t AstNodesDefaultCapacity       = 40960;
/// Начальное количество строительных узлов в ExtArray.
const size_t AstConstrNodeDefaultCapacity  = 64;
/// Начальное значение параметров функции.
const size_t AstFunctParamsDefaultCapacity = 8;
/// Начальное значение инструкций внутри функции.
const size_t AstFunctBodyDefaultCapacity   = 256;
/// Начальное значение количества инструкций в if-блоке.
const size_t AstIfBodyDefaultCapacity      = 64;
/// Начальное значение количества инструкций в while-блоке.
const size_t AstWhileBodyDefaultCapacity   = 64;

/// Особые ситуации при работе модуля АСД.
enum class AstError
{
	/// Нет ошибок.
	NoErrors,
	/// Ошибка при выполнении конструктора ExtArray.
	ExtArray,
	/// Ошибка при выполнении конструктора ExtHeap.
	ExtHeap,
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include "expr_nodes.h"
#include "funct_nodes.h"
#include "keyword_nodes.h"
#include "aux_nodes.h"
#include "ast_private.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Конструктор АСД.
 * 
 * @param ast Указатель на АСД.
 * 
 * @return AstError::ExtArray,
 *         AstError::ExtHeap,
 *         AstError::NoErrors.
*/
AstError AstConstructor(AST* const ast);

/**
 * @brief  Деструктор АСД.
 * 
 * @param ast Указатель на АСД.
*/
void AstDestructor(AST* const ast);

/**
 * @brief  Получить символьное представление названия типа.
 * 
 * @return Указатель на символьное представление.
*/
const char* const AstNodeGetName(const AstNodeType type);


ProgramStatus AstGraphicDump(const AST* ast);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !ABSTRACT_SYNTAX_TREE_H