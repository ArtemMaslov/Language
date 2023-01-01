#ifndef AST_VERIFY_PATTERNS_H
#define AST_VERIFY_PATTERNS_H

static const char* const DumpGraphHeader =
"digraph G\n"
"{\n"
	"\tgraph[dpi = 100];\n"
//	"\tsplines   = ortho;\n"
//	"\tranksep   = 1;\n"
	"\tbgcolor   = \"#303030\";\n\n"

	"\tedge\n"
	"\t[\n"
//		"\t\tminlen   = 3,\n"
		"\t\tpenwidth = 3,\n"
		"\t\tcolor    = \"green\"\n"
	"\t];\n\n"

	"\tnode\n"
	"\t[\n"
		"\t\tshape     = \"record\",\n"
		"\t\tstyle     = \"rounded, filled\",\n"
		"\t\tfillcolor = \"#404040\",\n"
		"\t\tfontsize  = 18,\n"
		"\t\tpenwidth  = 1,\n"
		"\t\tcolor     = \"#F9D423\",\n"
//		"\t\tmargin    = \"0.05 0.4\",\n"
		"\t\tfontcolor = \"#E1F5C4\"\n"
	"\t]\n\n";

static const char* const DumpEdgeFormat = "\t%s%zd -> %s%zd;\n";

static const char* const DumpAstNodeName = "astNode";
static const char* const DumpAstFormat =
	"\tastNode%zd[label = \"\n"
	"\t{\n"
		"\t\t{ AstRoot } |\n"
		"\t\t{ Nodes\\nextArray\\<constr\\>\\n }\n"
	"\t}\"];\n\n";

static const char* const DumpConstrNodeName = "constrNode";
static const char* const DumpConstrFormat =
	"\tconstrNode%zd[label = \"\n"
	"\t{\n"
		"\t\t{ Construction } |\n"
		"\t\t{ %s }\n"
	"\t}\"];\n\n";

static const char* const DumpInstrNodeName = "instrNode";
static const char* const DumpInstrFormat =
	"\tinstrNode%zd[label = \"\n"
	"\t{\n"
		"\t\t{ Instruction } |\n"
		"\t\t{ %s }\n"
	"\t}\"];\n\n";

static const char* const DumpVariableNodeName = "variableNode";
static const char* const DumpVariableFormat =
	"\tvariableNode%zd [label = \"\n"
	"\t{\n"
		"\t\t{ Variable } |\n"
		"\t\t{ %s } |\n"
		"\t\t{ InitValue }\n"
	"\t}\"];\n\n";

static const char* const DumpBinOperNodeName = "binOperNode";
static const char* const DumpBinOperFormat =
	"\tbinOperNode%zd [label = \"\n"
	"\t{\n"
		"\t\t{ BinaryOperator } |\n"
		"\t\t{ %s } |\n"
		"\t\t{ {Left} | {Right} }\n"
	"\t}\"];\n\n";

static const char* const DumpUnOperNodeName = "unarOperNode";
static const char* const DumpUnOperFormat =
	"\tunarOperNode%zd [label = \"\n"
	"\t{\n"
		"\t\t{ UnaryOperator } |\n"
		"\t\t{ %s } |\n"
		"\t\t{ Operand }\n"
	"\t}\"];\n\n";

static const char* const DumpExprNodeName = "exprNode";
static const char* const DumpExprFormat =
	"\texprNode%zd [label = \"\n"
	"\t{\n"
		"\t\t{ Expression } |\n"
		"\t\t{ %s }\n"
	"\t}\"];\n\n";

static const char* const DumpExprNumFormat =
	"\texprNode%zd [label = \"\n"
	"\t{\n"
		"\t\t{ Expression } |\n"
		"\t\t{ %s\\n%lf }\n"
	"\t}\"];\n\n";

static const char* const DumpFunctParamNodeName = "functParamNode";
static const char* const DumpFunctParamFormat =
	"\tfunctParamNode%zd [label = \"\n"
	"\t{\n"
		"\t\t{ FunctParam } |\n"
		"\t\t{ %s } |\n"
		"\t\t{ Value }\n"
	"\t}\"];\n\n";

static const char* const DumpFunctDefNodeName = "functDefNode";
static const char* const DumpFunctDefFormat =
	"\tfunctDefNode%zd [label = \"\n"
	"\t{\n"
		"\t\t{ FunctDef } |\n"
		"\t\t{ %s } |\n"
		"\t\t{ Params\\nextArr\\<functParamDef\\> } |\n"
		"\t\t{ Body\\nextArr\\<instr\\> }\n"
	"\t}\"];\n\n";

static const char* const DumpFunctCallNodeName = "functCallNode";
static const char* const DumpFunctCallFormat =
	"\tfunctCallNode%zd [label = \"\n"
	"\t{\n"
		"\t\t{ FunctCall } |\n"
		"\t\t{ %s } |\n"
		"\t\t{ Params\\nextArr\\<functParamDef\\> }\n"
	"\t}\"];\n\n";

static const char* const DumpIfNodeName = "ifNode";
static const char* const DumpIfFormat =
	"\tifNode%zd [label = \"\n"
	"\t{\n"
		"\t{ If } |\n"
		"\t{ Condition } |\n"
		"\t{ True\\nextArr\\<instr\\> } |\n"
		"\t{ False\\nextArr\\<instr\\> }\n"
	"\t}\"];\n\n";

static const char* const DumpWhileNodeName = "whileNode";
static const char* const DumpWhileFormat =
	"\twhileNode%zd [label = \"\n"
	"\t{\n"
		"\t\t{ While } |\n"
		"\t\t{ Condition } |\n"
		"\t\t{ Body\\nextArr\\<instr\\> }\n"
	"\t}\"];\n\n";

static const char* const DumpInputNodeName = "inputNode";
static const char* const DumpInputFormat =
	"\tinputNode%zd [label = \"\n"
	"\t{\n"
		"\t{ Input } |\n"
		"\t{ Variable }\n"
	"\t}\"];\n\n";

static const char* const DumpOutputNodeName = "outputNode";
static const char* const DumpOutputFormat =
	"\toutputNode%zd [label = \"\n"
	"\t{\n"
		"\t{ Output } |\n"
		"\t{ Expression }\n"
	"\t}\"];\n\n";

static const char* const DumpReturnNodeName = "returnNode";
static const char* const DumpReturnFormat =
	"\treturnNode%zd [label = \"\n"
	"\t{\n"
		"\t{ Return } |\n"
		"\t{ Value }\n"
	"\t}\"];\n\n";

#endif // !AST_VERIFY_PATTERNS_H