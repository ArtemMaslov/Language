#include <assert.h>

#include "AST.h"

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define CHECK_STATUS \
	if (status != ProgramStatus::Ok) \
		return status

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

ProgramStatus AstConstructor(AST* ast)
{
	assert(ast);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayConstructor(&ast->ConstrNodes, sizeof(ConstructionNode), AstConstrNodeDefaultCapacity);
	CHECK_STATUS;

	status = ExtHeapConstructor(&ast->Nodes, AST_NODES_HEAP_DEFAULT_SIZE);

	return status;
}

ProgramStatus AstDestructor(AST* ast)
{
	assert(ast);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayDestructor(&ast->ConstrNodes);
	CHECK_STATUS;

	status = ExtHeapDestructor(&ast->Nodes);

	return status;
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

const char* AstNodeGetName(AstNodeTypes type)
{
	switch (type)
	{
		case AstNodeTypes::Variable:
			return "Variable";

		case AstNodeTypes::FunctDef:
			return "FunctDef";

		case AstNodeTypes::FunctCall:
			return "FunctCall";

		case AstNodeTypes::FunctParamNode:
			return "FunctParam";

		case AstNodeTypes::Expression:
			return "Expression";

		case AstNodeTypes::BinaryOperator:
			return "BinaryOper";

		case AstNodeTypes::UnaryOperator:
			return "UnaryOper";

		case AstNodeTypes::If:
			return "If";

		case AstNodeTypes::While:
			return "While";

		case AstNodeTypes::Input:
			return "Input";

		case AstNodeTypes::Output:
			return "Output";

		case AstNodeTypes::Instruction:
			return "Instruction";

		case AstNodeTypes::Construction:
			return "Construction";

		case AstNodeTypes::Number:
			return "Number";

		case AstNodeTypes::Return:
			return "Return";

		default:
			assert(!"Unknown");
	}
}

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 