#ifndef AUXILIARY_NODE_H
#define AUXILIARY_NODE_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

struct ConstructionNode
{
	AstNodeTypes Type;

	union
	{
		void*         Ptr;
		FunctDefNode* FunctCall;
		VariableNode* GlobVar;
	} Node;
};

struct InstructionNode
{
	AstNodeTypes Type;

	union
	{
		void*          Ptr;
		VariableNode*  Variable;
		FunctCallNode* FunctCall;
		IfNode*        If;
		WhileNode*     While;
		InputNode*     Input;
		OutputNode*    Output;
		ReturnNode*    Return;
	} Node;
};

const size_t AST_CONSTR_NODE_DEFAULT_SIZE = 64;

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

/**
 * @brief �������� ������ ��� ConstructionNode � ExtHeap � �������������� �.
 * 
 * @param ast       ��������� �� ����������� �������������� ������.
 * @param functNode ��������� �� ���� ����������� �������.
 * 
 * @return ��������� �� ConstructionNode.
*/
ConstructionNode* AstCreateConstrNode(AST* ast, FunctDefNode* functNode);

/**
 * @brief �������� ������ ��� ConstructionNode � ExtHeap � �������������� �.
 *
 * @param ast         ��������� �� ����������� �������������� ������.
 * @param globVarNode ��������� �� ���� ����������� ���������� ����������.
 *
 * @return ��������� �� ConstructionNode.
*/
ConstructionNode* AstCreateConstrNode(AST* ast, VariableNode* globVarNode);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

InstructionNode* AstCreateInstructionNode(AST* ast, VariableNode* varNode);

InstructionNode* AstCreateInstructionNode(AST* ast, FunctCallNode* functCallNode);

InstructionNode* AstCreateInstructionNode(AST* ast, IfNode* ifNode);

InstructionNode* AstCreateInstructionNode(AST* ast, WhileNode* whileNode);

InstructionNode* AstCreateInstructionNode(AST* ast, InputNode* inputNode);

InstructionNode* AstCreateInstructionNode(AST* ast, OutputNode* outputNode);

InstructionNode* AstCreateInstructionNode(AST* ast, ReturnNode* retNode);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !AUXILIARY_NODE_H