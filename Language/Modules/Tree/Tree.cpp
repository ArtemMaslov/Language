#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <Windows.h>
#include <cmath>
#include <string.h>


#include "Tree.h"
#include "../Logs/Logs.h"


static double CalculateNode(const TreeNode* node, const Tree* tree, bool* canCalculate);

static void OptimizeNodeToNumber(TreeNode* node, const double number);

static bool TreeAllocNodes(Tree* tree);

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///                           Конструкторы и деструкторы
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

bool TreeConstructor(Tree* tree)
{
    assert(tree);

    tree->Root               = nullptr;
    
    tree->NodesArrayPtr      = (TreeNode*)calloc(MinNodesArraySize, sizeof(TreeNode));

    // #log

    if (!tree->NodesArrayPtr)
        return false; // #err

    tree->NodesArrayCapacity = MinNodesArraySize;
    tree->NodesArrayCurSize  = 0;

    return true; // #err
}

TreeNode* TreeNodeConstructor(Tree* tree, const TreeValue* value)
{
    assert(tree);
    //assert(expression);

    if (tree->NodesArrayCurSize >= tree->NodesArrayCapacity)
        if (!TreeAllocNodes(tree))
            return nullptr;

    size_t index = tree->NodesArrayCurSize;
    tree->NodesArrayPtr[index].NodeLeft  = nullptr;
    tree->NodesArrayPtr[index].NodeRight = nullptr;

    if (value)
        memcpy(&tree->NodesArrayPtr[index].Value, value, sizeof(TreeValue));

    tree->NodesArrayCurSize++;

    return tree->NodesArrayPtr + index;
}

bool TreeDestructor(Tree* problem) 
{
    // assert(tree);
    
    if (problem)
    {
        for (size_t st = 0; st < problem->NodesArrayCurSize; st++)
        {
            //ExpressionDestructor(&problem->NodesArrayPtr[st].Value);
        }

        TreeNodeDestructor(problem->Root);
        free(problem->NodesArrayPtr);
    }

    return true;
}

bool TreeNodeDestructor(TreeNode* node)
{
    // assert(node);

    if (node)
    {
        if (node->NodeLeft)
            TreeNodeDestructor(node->NodeLeft);
        if (node->NodeRight)
            TreeNodeDestructor(node->NodeRight);
    }

    return true;
}

static bool TreeAllocNodes(Tree* tree)
{
    assert(tree);

    assert(tree->NodesArrayCurSize >= tree->NodesArrayCapacity); // Проверяем на лишние не правильные вызовы

    TreeNode* nodes = (TreeNode*)realloc(tree->NodesArrayPtr, (size_t)(tree->NodesArrayCapacity * NodesArrayCapacityScale) * sizeof(TreeNode));

    if (!nodes)
    {
        // #log
        return false; // #err
    }

    tree->NodesArrayPtr = nodes;
    tree->NodesArrayCapacity *= NodesArrayCapacityScale;

    return true;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///                     Работа с деревом (стандартные функции)
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\

void TreeAddLeftNode(TreeNode* parent, TreeNode* child)
{
    assert(parent);
    assert(child);

    parent->NodeLeft = child;
    child->Parent = parent;
}

void TreeAddRightNode(TreeNode* parent, TreeNode* child)
{
    assert(parent);
    assert(child);

    parent->NodeRight = child;
    child->Parent = parent;
}

size_t TreeMeasure(TreeNode* node)
{
    assert(node);

    size_t result = 1;
    if (node->NodeLeft)
        result += TreeMeasure(node->NodeLeft);
    if (node->NodeRight)
        result += TreeMeasure(node->NodeRight);
    node->ChildCount = result;

    return result;
}

bool IsLeaf(const TreeNode* node)
{
    assert(node);

    if (node && (node->NodeLeft == nullptr || node->NodeRight == nullptr))
        return true;
    else
        return false;
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\\