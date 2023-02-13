///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
// Модуль абстрактного синтаксического дерева АСД (AST).
// 
// Конструктор, деструктор АСД и служебные функции.
//  
// Версия: 1.0.1.0
// Дата последнего изменения: 15:00 30.01.2023
// 
// Автор: Маслов А.С. (https://github.com/ArtemMaslov).
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

#include <assert.h>
#include <string.h>

#include "../Modules/Logs/Logs.h"
#include "../Modules/ErrorsHandling.h"

#include "AST.h"
#include "ast_private.h"

#define AST_NODE(type) #type,
static const char* const AstNodeNames[] =
{
	"Variable",
#include "ast_nodes.inc"
};
#undef AST_NODE

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

AstError AstConstructor(AST* const ast)
{
	assert(ast);

	ProgramStatus status = ProgramStatus::Ok;

	status = ExtArrayConstructor(&ast->ConstrNodes, sizeof(ConstructionNode), AstConstrNodeDefaultCapacity);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		return AstError::ExtArray;
	}

	status = ExtHeapConstructor(&ast->Nodes, AstNodesDefaultCapacity);
	if (status != ProgramStatus::Ok)
	{
		TRACE_ERROR();
		ExtArrayDestructor(&ast->ConstrNodes);
		return AstError::ExtHeap;
	}

	return AstError::NoErrors;
}

void AstDestructor(AST* const ast)
{
	assert(ast);

	AstConstrBlockDestructor(&ast->ConstrNodes);

	ExtArrayDestructor(&ast->ConstrNodes);
	ExtHeapDestructor(&ast->Nodes);
	memset(ast, 0, sizeof(AST));
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///

const char* const AstNodeGetName(const AstNodeType type)
{
	return AstNodeNames[(size_t)type];
}

///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///
///***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***///