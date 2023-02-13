///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
// 
// Функции работы с служебными узлами АСД: строительными узлами и инструкциями.
//  
// Версия: 1.0.1.0
// Дата последнего изменения: 15:58 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef AUXILIARY_NODE_H
#define AUXILIARY_NODE_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief Строительный узел.
 * 
 * Из строительных узлов состоит текст программы. Узлами могут быть функции и глобальные
 * переменные.
*/
struct ConstructionNode
{
	/// Тип узла.
	AstNodeType Type;

	/// Содержимое узла.
	union
	{
		/// Служебный указатель на узел. 
		void*         Ptr;
		/// Определение функции.
		FunctDefNode* FunctDef;
		/// Глобальная переменная.
		VariableNode* GlobVar;
	} Node;
};

/**
 * @brief Узел инструкции.
 * 
 * Из инструкций состоит тело функции. Инструкцией могут быть следующие узлы:
 * 1. Определение переменной.
 * 2. Вызов функции.
 * 3. Условный оператор.
 * 4. Оператор цикла.
 * 5. Оператор ввода.
 * 6. Оператор вывода.
 * 7. Оператор возврата из функции.
*/
struct InstructionNode
{
	/// Тип узла.
	AstNodeType Type;

	/// Содержимое узла.
	union
	{
		/// Служебный указатель на узел.
		void*          Ptr;
		/// Определение переменной.
		VariableNode*  Variable;
		/// Вызов функции.
		FunctCallNode* FunctCall;
		/// Условный оператор.
		IfNode*        If;
		/// Оператор цикла.
		WhileNode*     While;
		/// Оператор ввода.
		InputNode*     Input;
		/// Оператор вывода.
		OutputNode*    Output;
		/// Оператор возврата из функции.
		ReturnNode*    Return;
	} Node;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Создать строительный узел из определения функции.
 * 
 * @param ast       Указатель на АСД.
 * @param functNode Указатель на узел определения функции.
 * 
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
ConstructionNode* AstCreateConstrNode(AST* const ast, FunctDefNode* const functNode);

/**
 * @brief  Создать строительный узел из определения глобальной переменной.
 *
 * @param ast         Указатель на АСД.
 * @param globVarNode Указатель на узел определения глобальной переменной.
 *
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
ConstructionNode* AstCreateConstrNode(AST* const ast, VariableNode* const globVarNode);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Создать инструкцию из определения переменной.
 *
 * @param ast     Указатель на АСД.
 * @param varNode Указатель на узел определения переменной.
 *
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
InstructionNode* AstCreateInstructionNode(AST* const ast, VariableNode* const varNode);

/**
 * @brief  Создать инструкцию из вызова функции.
 *
 * @param ast           Указатель на АСД.
 * @param functCallNode Указатель на узел вызова функции.
 *
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
InstructionNode* AstCreateInstructionNode(AST* const ast, FunctCallNode* const functCallNode);

/**
 * @brief  Создать инструкцию из условного оператора.
 *
 * @param ast    Указатель на АСД.
 * @param ifNode Указатель на узел условного оператора.
 *
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
InstructionNode* AstCreateInstructionNode(AST* const ast, IfNode* const ifNode);

/**
 * @brief  Создать инструкцию из оператора цикла.
 *
 * @param ast       Указатель на АСД.
 * @param whileNode Указатель на узел оператора цикла.
 *
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
InstructionNode* AstCreateInstructionNode(AST* const ast, WhileNode* const whileNode);

/**
 * @brief  Создать инструкцию из оператора ввода.
 *
 * @param ast       Указатель на АСД.
 * @param inputNode Указатель на узел оператора ввода.
 *
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
InstructionNode* AstCreateInstructionNode(AST* const ast, InputNode* const inputNode);

/**
 * @brief  Создать инструкцию из оператора вывода.
 *
 * @param ast        Указатель на АСД.
 * @param outputNode Указатель на узел оператора вывода.
 *
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
InstructionNode* AstCreateInstructionNode(AST* const ast, OutputNode* const outputNode);

/**
 * @brief  Создать инструкцию из оператора возврата из функции.
 *
 * @param ast     Указатель на АСД.
 * @param retNode Указатель на узел оператора возврата из функции.
 *
 * @return nullptr, если не удалось выделить память.
 *         Указатель на созданный узел.
*/
InstructionNode* AstCreateInstructionNode(AST* const ast, ReturnNode* const retNode);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !AUXILIARY_NODE_H