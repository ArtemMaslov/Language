#define REG1(reg) \
	AsmCreateArgReg(&arg1, AsmRegType::reg)

#define REG2(reg) \
	AsmCreateArgReg(&arg2, AsmRegType::reg)

#define LABEL_ARG(name) \
	AsmCreateArgLabel(&arg1, name)

#define MEM1(reg, imm) \
	AsmCreateArgMem(&arg1, AsmRegType::reg, imm)

#define MEM2(reg, imm) \
	AsmCreateArgMem(&arg2, AsmRegType::reg, imm)

#define IMM1(imm) \
	AsmCreateArgImm(&arg1, imm)

#define IMM2(imm) \
	AsmCreateArgImm(&arg2, imm)

#define ARG1_TYPE(type) \
	(cmd->Arg1.Type == AsmArgType::type)

#define ARG2_TYPE(type) \
		(cmd->Arg2.Type == AsmArgType::type)

#define LABEL(name) \
	AsmCreateCommand(asmRep, AsmCmdType::label, LABEL_ARG(name))

#define POP(...) \
	AsmCreateCommand(asmRep, AsmCmdType::pop, __VA_ARGS__)

#define PUSH(...) \
	AsmCreateCommand(asmRep, AsmCmdType::push, __VA_ARGS__)

#define MOV(...) \
	AsmCreateCommand(asmRep, AsmCmdType::mov, __VA_ARGS__)

#define ADD(...) \
	AsmCreateCommand(asmRep, AsmCmdType::add, __VA_ARGS__)

#define SUB(...) \
	AsmCreateCommand(asmRep, AsmCmdType::sub, __VA_ARGS__)

#define IMUL(...) \
	AsmCreateCommand(asmRep, AsmCmdType::imul, __VA_ARGS__)

#define CDQ() \
	AsmCreateCommand(asmRep, AsmCmdType::cdq)

#define DIV(...) \
	AsmCreateCommand(asmRep, AsmCmdType::div, __VA_ARGS__)

#define IDIV(...) \
	AsmCreateCommand(asmRep, AsmCmdType::idiv, __VA_ARGS__)

#define XOR(...) \
	AsmCreateCommand(asmRep, AsmCmdType::_xor, __VA_ARGS__)

#define NEG(...) \
	AsmCreateCommand(asmRep, AsmCmdType::neg, __VA_ARGS__)

#define INC(...) \
	AsmCreateCommand(asmRep, AsmCmdType::inc, __VA_ARGS__)

#define DEC(...) \
	AsmCreateCommand(asmRep, AsmCmdType::dec, __VA_ARGS__)

#define CMP(...) \
	AsmCreateCommand(asmRep, AsmCmdType::cmp, __VA_ARGS__)

#define JMP(label) \
	AsmCreateCommand(asmRep, AsmCmdType::jmp, LABEL_ARG(label))

#define JE(label) \
	AsmCreateCommand(asmRep, AsmCmdType::je, LABEL_ARG(label))

#define JNE(label) \
	AsmCreateCommand(asmRep, AsmCmdType::jne, LABEL_ARG(label))

#define JA(label) \
	AsmCreateCommand(asmRep, AsmCmdType::ja, LABEL_ARG(label))

#define JAE(label) \
	AsmCreateCommand(asmRep, AsmCmdType::jae, LABEL_ARG(label))

#define JB(label) \
	AsmCreateCommand(asmRep, AsmCmdType::jb, LABEL_ARG(label))

#define JBE(label) \
	AsmCreateCommand(asmRep, AsmCmdType::jbe, LABEL_ARG(label))

#define JG(label) \
	AsmCreateCommand(asmRep, AsmCmdType::jg, LABEL_ARG(label))

#define JGE(label) \
	AsmCreateCommand(asmRep, AsmCmdType::jge, LABEL_ARG(label))

#define JL(label) \
	AsmCreateCommand(asmRep, AsmCmdType::jl, LABEL_ARG(label))

#define JLE(label) \
	AsmCreateCommand(asmRep, AsmCmdType::jle, LABEL_ARG(label))

#define CALL(label) \
	AsmCreateCommand(asmRep, AsmCmdType::call, LABEL_ARG(label))

#define RET() \
	AsmCreateCommand(asmRep, AsmCmdType::ret)

#define CVTSI2SD(...) \
	AsmCreateCommand(asmRep, AsmCmdType::cvtsi2sd, __VA_ARGS__)

#define MOVSD(...) \
	AsmCreateCommand(asmRep, AsmCmdType::movsd, __VA_ARGS__)

#define FLD(...) \
	AsmCreateCommand(asmRep, AsmCmdType::fld, __VA_ARGS__)

#define FSTP(...) \
	AsmCreateCommand(asmRep, AsmCmdType::fstp, __VA_ARGS__)

#define FSTPD(...) \
	AsmCreateCommand(asmRep, AsmCmdType::fstpd, __VA_ARGS__)

#define FADDP() \
	AsmCreateCommand(asmRep, AsmCmdType::faddp)

#define FSUBP() \
	AsmCreateCommand(asmRep, AsmCmdType::fsubp)

#define FMULP() \
	AsmCreateCommand(asmRep, AsmCmdType::fmulp)

#define FDIVP() \
	AsmCreateCommand(asmRep, AsmCmdType::fdivp)

#define FCHS() \
	AsmCreateCommand(asmRep, AsmCmdType::fchs)

#define FCOMPP() \
	AsmCreateCommand(asmRep, AsmCmdType::fcompp)

#define FRND() \
	AsmCreateCommand(asmRep, AsmCmdType::frnd)

#define FISTP(...) \
	AsmCreateCommand(asmRep, AsmCmdType::fistp, __VA_ARGS__)