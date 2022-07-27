#ifndef TREE_H
#define TREE_H

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

const double NodesArrayCapacityScale = 1.5;
const size_t MinNodesArraySize       = 100;

typedef int TreeValue;

struct TreeNode
{
    /// Значение узла.
    TreeValue Value;
    /// Родитель узла.
    TreeNode* Parent;
    /// Левый потомок.
    TreeNode* NodeLeft;
    /// Правый потомок.
    TreeNode* NodeRight;
    /// Количество потомков у поддерева.
    size_t    ChildCount;
};

struct Tree
{
    /// Корень дерева.
    TreeNode* Root;

    /// Указатель на начало массива узлов
    TreeNode* NodesArrayPtr;
    /// Размер массива
    size_t    NodesArrayCurSize;
    /// Максимальное количество элементов в массиве до увелечения памяти.
    size_t    NodesArrayCapacity;
};

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(Tree* programm);

TreeNode* TreeNodeConstructor(Tree* tree, const TreeValue* value);

bool TreeDestructor(Tree* programm);

bool TreeNodeDestructor(TreeNode* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

void TreeAddRightNode(TreeNode* Parent, TreeNode* child);

void TreeAddLeftNode(TreeNode* Parent, TreeNode* child);

size_t TreeMeasure(TreeNode* node);

bool IsLeaf(const TreeNode* node);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

#endif // !TREE_H