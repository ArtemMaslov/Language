///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
// 
// Функции поддержки встроенных в язык операторов.
//  
// Версия: 1.0.1.0
// Дата последнего изменения: 18:40 03.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef KEYWORD_NODES_H
#define KEYWORD_NODES_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/// Узел условного оператора.
struct IfNode
{
	/// Выражение, проверяемое на истинность.
	ExpressionNode* Condition;
	/**
	 * @brief Инструкции, выполняемые, если выражение истинно.
	 * ExtArray<InstructionNode>
	 */
	ExtArray TrueBlock;
	/**
	 * @brief Инструкции, выполняемые, если выражение ложно.
	 * ExtArray<InstructionNode>
	 */
	ExtArray FalseBlock;
};

/// Узел оператора цикла.
struct WhileNode
{
	/// Выражение, проверяемое на истинность.
	ExpressionNode* Condition;
	/**
	 * @brief Циклически выполняемые инструкции.
	 * ExtArray<InstructionNode>
	 */
	ExtArray Body;
};

/// Узел оператора ввода.
struct InputNode
{
	/// Переменная, в которое нужно записать считанное значение.
	VariableNode*  Input;
};

/// Узел оператора вывода.
struct OutputNode
{
	/// Выражение, которое нужно вывести в поток стандартного вывода.
	ExpressionNode* Output;
};

/// Узел оператора возврата.
struct ReturnNode
{
	/// Выражение, возвращаемое из функции.
	ExpressionNode* Value;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Создать узел оператора ввода.
 * 
 * @param ast     Указатель на АСД.
 * @param varNode Переменная, в которую нужно записать считанное значение.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
 */
InputNode* AstCreateInputNode(AST* const ast, VariableNode* const varNode);

/**
 * @brief  Создать узел оператора вывода.
 * 
 * @param ast      Указатель на АСД.
 * @param exprNode Выводимое в поток стандартного вывода выражение.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
 */
OutputNode* AstCreateOutputNode(AST* const ast, ExpressionNode* const exprNode);

/**
 * @brief  Создать узел условного оператора.
 * 
 * @param ast       Указатель на АСД.
 * @param condition Условное выражение.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
 */
IfNode* AstCreateIfNode(AST* const ast, ExpressionNode* const condition);

/**
 * @brief  Создать узел оператора цикла.
 * 
 * @param ast       Указатель на АСД.
 * @param condition Условное выражение.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
 */
WhileNode* AstCreateWhileNode(AST* const ast, ExpressionNode* const condition);

/**
 * @brief  Создать узел оператора возврата.
 * 
 * @param ast      Указатель на АСД.
 * @param exprNode Возвращаемое выражение.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
 */
ReturnNode* AstCreateReturnNode(AST* const ast, ExpressionNode* const exprNode);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !KEYWORD_NODES_H