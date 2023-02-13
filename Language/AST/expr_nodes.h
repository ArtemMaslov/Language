///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
// 
// Функции работы с узлами выражений.
//  
// Версия: 1.0.1.0
// Дата последнего изменения: 15:33 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef EXPRESSION_NODES_H
#define EXPRESSION_NODES_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief Узел переменной.
 * 
 * Использование узла:
 * 1. Определение переменной. InitValue != nullptr.
 * 2. Использование переменных в выражениях. InitValue != nullptr.
*/
struct VariableNode
{
	/// Идентификатор переменной.
	size_t NameId;
	/// Начальное значение.
	ExpressionNode* InitValue;
};

/// Узел бинарного оператора.
struct BinaryOperatorNode
{
	/// Тип оператора.
	OperatorType Operator;

	/// Левый операнд.
	ExpressionNode* LeftOperand;
	/// Правый операнд.
	ExpressionNode* RightOperand;
};

/// Узел унарного оператора.
struct UnaryOperatorNode
{
	/// Тип оператора.
	OperatorType Operator;

	/// Операнд.
	ExpressionNode* Operand;
};

/**
 * @brief Узел выражения.
 * 
 * Выражение является основным структурным элементом для вычислительных операций в языке.
 * Выражением могут быть следующие узлы:
 * 1. Бинарный оператор.
 * 2. Унарный оператор.
 * 3. Число.
 * 4. Вызов функции.
 * 5. Переменная.
*/
struct ExpressionNode
{
	/// Тип выражения.
	AstNodeType Type;

	/// Содержимое узла.
	union
	{
		/// Служебный указатель на узел.
		void*               Ptr;
		/// Бинарный оператор.
		BinaryOperatorNode* BinaryOperator;
		/// Унарный оператор.
		UnaryOperatorNode*  UnaryOperator;
		/// Число.
		double              Number;
		/// Вызов функции.
		FunctCallNode*      FunctCall;
		/// Определение переменный.
		VariableNode*    Variable;
	} Node;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Создать выражение из числа.
 * 
 * @param ast    Указатель на АСД.
 * @param number Число.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
ExpressionNode* AstCreateExpressionNode(AST* const ast, const double number);

/**
 * @brief  Создать выражение из бинарного оператора.
 * 
 * @param ast     Указатель на АСД.
 * @param binOper Указатель на узел бинарного оператора.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
ExpressionNode* AstCreateExpressionNode(AST* const ast, BinaryOperatorNode* const binOper);

/**
 * @brief  Создать выражение из унарного оператора.
 * 
 * @param ast        Указатель на АСД.
 * @param unOperNode Указатель на узел унарного оператора.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
ExpressionNode* AstCreateExpressionNode(AST* const ast, UnaryOperatorNode* const unOperNode);

/**
 * @brief  Создать выражение из вызова функции.
 * 
 * @param ast           Указатель на АСД.
 * @param functCallNode Указатель на узел вызова функции.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
ExpressionNode* AstCreateExpressionNode(AST* const ast, FunctCallNode* const functCallNode);

/**
 * @brief  Создать выражение из переменной.
 * 
 * @param ast     Указатель на АСД.
 * @param varNode Указатель на узел переменной.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
ExpressionNode* AstCreateExpressionNode(AST* const ast, VariableNode* const varNode);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Создать унарный оператор.
 * 
 * @param ast     Указатель на АСД.
 * @param oper    Тип унарного оператора.
 * @param operand Указатель на выражение операнда.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
UnaryOperatorNode* AstCreateUnaryOperatorNode(AST* const ast, const OperatorType oper, ExpressionNode* const operand);

/**
 * @brief  Создать бинарный оператор.
 * 
 * @param ast          Указатель на АСД.
 * @param oper         Тип бинарного оператора.
 * @param leftOperand  Указатель на выражение левого операнда.
 * @param rightOperand Указатель на выражение правого операнда.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
BinaryOperatorNode* AstCreateBinaryOperatorNode(AST* const ast, const OperatorType oper,
												ExpressionNode* const leftOperand, ExpressionNode* const rightOperand);

/**
 * @brief  Создать узел переменной.
 * 
 * @param ast       Указатель на АСД.
 * @param nameId    Номер идентификатора переменной.
 * @param initValue Инициализирующее выражение.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
VariableNode* AstCreateVariableNode(AST* const ast, const size_t nameId, ExpressionNode* const initValue);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !EXPRESSION_NODES_H