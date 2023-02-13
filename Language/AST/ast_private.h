///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
// 
// Конструкторы и деструкторы некоторых узлов. Файл является приватным, так как модуль
// сам следит за вызовом конструкторов и деструкторов при работе с АСД.
//  
// Версия: 1.0.0.1
// Дата последнего изменения: 18:40 03.02.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#ifndef AST_PRIVATE_H
#define AST_PRIVATE_H

#include "AST.h"

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

/**
 * @brief  Деструктор строительного узла.
 * 
 * @param node Указатель на строительный узел.
 */
void AstConstrNodeDestructor(ConstructionNode* const node);

/**
 * @brief  Деструктор блока строительных узлов.
 * 
 * Вызывает деструктор для каждого строительного узла блока.
 * 
 * @param constrBlock Указатель на блок строительных узлов. ExtArray<ConstructionNode>.
 */
void AstConstrBlockDestructor(ExtArray* const constrBlock);

/**
 * @brief  Деструктор узла инструкции.
 * 
 * @param node Указатель на инструкцию.
 */
void AstInstrNodeDestructor(InstructionNode* const node);

/**
 * @brief  Деструктор блока инструкций.
 * 
 * Вызывает деструктор для каждой инструкции блока.
 * 
 * @param constrBlock Указатель на блок инструкций. ExtArray<InstructionNode>.
 */
void AstInstrBlockDestructor(ExtArray* const instrBlock);

/**
 * @brief  Конструктор узла определения функции.
 * 
 * @param node Указатель на узел определения функции.
 * 
 * @return AstError::ExtArray,
 *         AstError::NoErrors.
*/
AstError AstFunctDefNodeConstructor(FunctDefNode* const node);

/**
 * @brief  Деструктор узла определения функции.
 * 
 * @param node Указатель на узел определения функции.
*/
void AstFunctDefNodeDestructor(FunctDefNode* const node);

/**
 * @brief  Конструктор узла вызова функции.
 * 
 * @param node Указатель на узел вызова функции.
 * 
 * @return AstError::ExtArray,
 *         AstError::NoErrors.
*/
AstError AstFunctCallNodeConstructor(FunctCallNode* const node);

/**
 * @brief  Деструктор узла вызова функции.
 * 
 * @param node Указатель на узел вызова функции.
*/
void AstFunctCallNodeDestructor(FunctCallNode* const node);

/**
 * @brief  Конструктор узла оператора цикла.
 * 
 * @param node Указатель на узел оператора цикла.
 * 
 * @return AstError::ExtArray,
 *         AstError::NoErrors.
 */
AstError AstWhileNodeConstructor(WhileNode* node);

/**
 * @brief  Деструктор узла оператора цикла.
 * 
 * @param node Указатель на узел оператора цикла.
 */
void AstWhileNodeDestructor(WhileNode* node);

/**
 * @brief  Конструктор узла условного оператора.
 * 
 * @param node Указатель на узел условного оператора.
 * 
 * @return AstError::ExtArray,
 *         AstError::NoErrors.
 */
AstError AstIfNodeConstructor(IfNode* node);

/**
 * @brief  Деструктор узла условного оператора.
 * 
 * @param node Указатель на узел условного оператора.
 */
void AstIfNodeDestructor(IfNode* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#endif // !AST_PRIVATE_H