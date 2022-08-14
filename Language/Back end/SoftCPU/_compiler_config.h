#ifndef COMPILER_CONFIG_H
#define COMPILER_CONFIG_H

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#define SET_VAR_VALUE "\tpop [cx %+d]\n"
#define GET_VAR_VALUE "\tpush [cx %+d]\n"

#define INC_BX    \
	"\tpush bx\n" \
	"\tpush 1\n"  \
	"\tadd\n"	  \
	"\tpop bx\n"

//***///***///---\\\***\\\***\\\___/// Arithmetic \\\___///***///***///---\\\***\\\***\\ 

static const char* const CompAdd = "\tadd\n\n";
static const char* const CompSub = "\tsub\n\n";
static const char* const CompMul = "\tmul\n\n";
static const char* const CompDiv = "\tdiv\n\n";

//***///***///---\\\***\\\***\\\___/// Condition \\\___///***///***///---\\\***\\\***\\ 

static const char* const CompCompare = 
	"\t%s l_%s_false_%zd\n"
	"\tpush 1\n"
	"\tjmp l_%s_end_%zd\n"
"l_%s_false_%zd:\n"
	"\tpush 0\n"
"l_%s_end_%zd:\n\n";

static const char* const CompEqualJmpType          = "jne";
static const char* const CompEqualLabelName        = "equal";

static const char* const CompNotEqualJmpType       = "je";
static const char* const CompNotEqualLabelName     = "not_equal";

static const char* const CompLessJmpType           = "jae";
static const char* const CompLessLabelName         = "less";

static const char* const CompLessEqualJmpType      = "ja";
static const char* const CompLessEqualLabelName    = "less_equal";

static const char* const CompGreaterJmpType        = "jbe";
static const char* const CompGreaterLabelName      = "greater";

static const char* const CompGreaterEqualJmpType   = "jb";
static const char* const CompGreaterEqualLabelName = "greater_equal";

//***///***///---\\\***\\\***\\\___/// Logical \\\___///***///***///---\\\***\\\***\\ 

static const char* const CompNot = 
	"\tpush 0\t"
	"\tjne l_not_false_%zd\n"
	"\tpush 1\n"
	"\tjmp l_not_end_%zd\n"
"l_not_false_%zd:\n"
	"\tpush 0\n"
"l_not_end_%zd:\n\n";

static const char* const CompAnd = 
	"\tpush 1\n"
	"\tjne l_and_false_%zd\n"
	"\tpush 1\n"
	"\tjne l_and_false_%zd\n"
	"\tpush 1\n"
	"\tjmp l_and_end_%zd\n"
"l_and_false_%zd:\n"
	"\tpush 0\n"
"l_and_end_%zd:\n";

static const char* const CompOr = 
	"\tpush 1\n"
	"\tje l_or_true_%zd\n"
	"\tpush 1\n"
	"\tjne l_or_false_%zd\n"
"l_or_true_%zd:\n"
	"\tpush 1\n"
	"\tjmp l_or_end_%zd\n"
"l_or_false_%zd:\n"
	"\tpush 0\n"
"l_or_end_%zd:\n";

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static const char* const CompSqrt = "\tsqrt\n\n";

static const char* const CompFunctDefStart =
"%s:\n"
	"\tpush bx\n"
	"\tpush cx\n"
	"\tpush bx\n"
	"\tpop cx\n\n";

static const char* const CompFunctDefEnd =
	"\tpop cx\n"
	"\tpop bx\n"
	"\tret\n\n";

static const char* const CompFunctCallSaveParam =
	"\tpop [bx]\n"
	INC_BX "\n";

static const char* const CompFunctCall = 
	"\tcall %s\n"
	"\tpush bx\n"
	"\tpush %zd\n"
	"\tsub\n"
	"\tpop bx\n\n";

static const char* const CompRetValue = "\tpop ax\n";

static const char* const CompGetRetValue = "\tpush ax\n\n";

static const char* const CompVarSetValue = SET_VAR_VALUE "\n";

static const char* const CompVarInit = SET_VAR_VALUE INC_BX "\n";

static const char* const CompVarGetValue = GET_VAR_VALUE "\n";

static const char* const CompNumber = 
	"\tpush %lf\n\n";

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static const char* const CompInput = 
	"\tin\n"
	SET_VAR_VALUE "\n";

static const char* const CompOutput =
	"\tout\n\n";

//***///***///---\\\***\\\***\\\___///    If   \\\___///***///***///---\\\***\\\***\\ 

static const char* const CompIfConditionFormat = 
	"\tpush 1\n"
	"\tjne l_if_false_%zd\n\n";

static const char* const CompIfFalseLabelFormat = 
	"\tjmp l_if_end_%zd\n"
"l_if_false_%zd:\n\n";

static const char* const CompIfEndLabelFormat = 
"l_if_end_%zd:\n\n";

//***///***///---\\\***\\\***\\\___///  While  \\\___///***///***///---\\\***\\\***\\ 

static const char* const CompWhileConditionLabelFormat = 
"l_while_cond_%zd:\n\n";

static const char* const CompWhileConditionCheckFormat =
	"\tpush 1\n"
	"\tjne l_while_end_%zd\n\n";

static const char* const CompWhileEndFormat =
	"\tjmp l_while_cond_%zd\n"
"l_while_end_%zd:\n\n";

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

static const char* const CompUnSubFormat = 
	"\tpop ax\n"
	"\tpush 0\n"
	"\tpush ax\n"
	"\tsub\n\n";

//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 
//***///***///---\\\***\\\***\\\___///***___***\\\___///***///***///---\\\***\\\***\\ 

#endif // !COMPILER_CONFIG_H