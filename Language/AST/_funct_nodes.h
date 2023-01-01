#ifndef FUNCT_NODE_H
#define FUNCT_NODE_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

struct FunctParamNode
{
	int NameId;
	ExpressionNode* Value;
	// ParamType;
	// VariableNumberOfParams
};

struct FunctDefNode
{
	int NameId;
	// ExtArray<FunctParamNode>
	ExtArray Params;
	ExtArray Body;
};

struct FunctCallNode
{
	int NameId;

	// ExtArray<FunctParamNode>
	ExtArray Params;
};

const size_t AstFunctParamsDefaultCapacity = 16;
const size_t AstFunctBodyDefaultCapacity   = 256;

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus AstFunctDefNodeConstructor(FunctDefNode* node);

ProgramStatus AstFunctDefNodeDestructor(FunctDefNode* node);

ProgramStatus AstFunctCallNodeConstructor(FunctCallNode* node);

ProgramStatus AstFunctCallNodeDestructor(FunctCallNode* node);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

FunctDefNode* AstCreateFunctDefNode(AST* ast, int nameId);

FunctCallNode* AstCreateFunctCallNode(AST* ast, int nameId);

FunctParamNode* AstCreateFunctParamNode(AST* ast, ExpressionNode* expr);

FunctParamNode* AstCreateFunctParamNode(AST* ast, int nameId);

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !FUNCT_NODE_H