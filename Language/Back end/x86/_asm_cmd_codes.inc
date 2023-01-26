ASM_CMD(push, "push", /*cmd name*/
		Reg, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE1(0b01010000), /*code*/
		0, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(push, "push", /*cmd name*/
		Imm, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE1(0b01101000), /*code*/
		1, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(push, "push", /*cmd name*/
		Mem, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11111111, 0b00110000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		ModeCodeRm    /*modeRmType*/)

ASM_CMD(pop, "pop",   /*cmd name*/
		Reg, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE1(0b01011000), /*code*/
		0, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(pop, "pop",   /*cmd name*/
		Mem, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b10001111, 0b00000000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		ModeCodeRm    /*modeRmType*/)

ASM_CMD(mov, "mov",   /*cmd name*/
		Reg, Reg,     /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b10001010, 0b11000000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeRegReg    /*modeRmType*/)

ASM_CMD(mov, "mov",   /*cmd name*/
		Reg, Imm,     /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11000110, 0b11000000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(add, "add",   /*cmd name*/
		Reg, Reg,     /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00000010, 0b11000000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeRegReg    /*modeRmType*/)

ASM_CMD(add, "add",   /*cmd name*/
		Reg, Imm,     /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b10000000, 0b11000000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(sub, "sub",   /*cmd name*/
		Reg, Reg,     /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00101010, 0b11000000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeRegReg    /*modeRmType*/)

ASM_CMD(sub, "sub",   /*cmd name*/
		Reg, Imm,     /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b10000000, 0b11101000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(mul, "mul",   /*cmd name*/
		Reg, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11110110, 0b11100000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(imul, "imul", /*cmd name*/
		Reg, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11110110, 0b11101000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(div, "div",   /*cmd name*/
		Reg, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11110110, 0b11110000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(idiv, "idiv", /*cmd name*/
		Reg, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11110110, 0b11111000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(xor_, "xor",   /*cmd name*/
		Reg, Reg,     /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00110010, 0b11000000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeRegReg    /*modeRmType*/)

ASM_CMD(neg, "neg",   /*cmd name*/
		Reg, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11110110, 0b11011000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(cdq, "cdq",   /*cmd name*/
		Null, Null,   /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE1(0b10011001), /*code*/
		1, 0, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(cmp, "cmp",   /*cmd name*/
		Reg, Reg,     /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00111000, 0b11000000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeRegReg    /*modeRmType*/)

ASM_CMD(cmp, "cmp",   /*cmd name*/
		Imm, Reg,     /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b10000000, 0b11111000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		CodeW,        /*opcodeLastByte*/
		CodeReg       /*modeRmType*/)

ASM_CMD(jmp, "jmp",   /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE1(0b11101001), /*code*/
		1, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(je, "je",     /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10000100), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(jne, "jne",   /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10000101), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(ja, "ja",     /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10000111), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(jae, "jae",   /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10000011), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(jb, "jb",     /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10000010), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(jbe, "jbe",   /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10000110), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(jg, "jg",     /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10001111), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(jge, "jge",   /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10001101), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(jl, "jl",     /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10001100), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(jle, "jle",   /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b00001111, 0b10001110), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(call, "call", /*cmd name*/
		Label, Null,  /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE1(0b11101000), /*code*/
		1, 0, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(ret, "ret",   /*cmd name*/
		Null, Null,   /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE1(0b11000011), /*code*/
		1, 0, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(fchs, "fchs", /*cmd name*/
		Null, Null,   /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11011001, 0b11100000), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(fld, "fld",   /*cmd name*/
		Mem, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11011001, 0b00000000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		ModeCodeRm    /*modeRmType*/)

ASM_CMD(fstp, "fstp", /*cmd name*/
		Mem, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11011001, 0b00011000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		ModeCodeRm    /*modeRmType*/)

ASM_CMD(fstpd, "fstpd", /*cmd name*/
		Mem, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11011101, 0b00011000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		ModeCodeRm    /*modeRmType*/)

ASM_CMD(fistp, "fistp", /*cmd name*/
		Mem, Null,    /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11011011, 0b00011000), /*code*/
		1, 1, 0,      /*Size: opcode modeRm sib*/
		4,            /*immSize*/
		Code,         /*opcodeLastByte*/
		ModeCodeRm    /*modeRmType*/)

// fcompp
// cmd->Code.Code[0]     = 0b11011110;
// cmd->Code.Code[1]     = 0b11011001;
// fstsw ax
// cmd->Code.Code[2]     = 0b11011111;
// cmd->Code.Code[3]     = 0b11100000;
// sahf
// cmd->Code.Code[4]     = 0b10011110;
ASM_CMD(fcompp, "fcompp", /*cmd name*/
		Null, Null,   /*arg1 arg2*/
		BYTE4(0b11011110, 0b11011001, 0b11011111, 0b11100000), /*prefixes*/
		4,            /*prefixesSize*/
		BYTE1(0b10011110), /*code*/
		1, 0, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(frnd, "frnd", /*cmd name*/
		Null, Null,   /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b10011110, 0b11111100), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(faddp, "faddp", /*cmd name*/
		Null, Null,   /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11011110, 0b11000001), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(fsubp, "fsubp", /*cmd name*/
		Null, Null,   /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11011110, 0b11101001), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(fmulp, "fmulp", /*cmd name*/
		Null, Null,   /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11011110, 0b11001001), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)

ASM_CMD(fdivp, "fdivp", /*cmd name*/
		Null, Null,   /*arg1 arg2*/
		{},           /*prefixes*/
		0,            /*prefixesSize*/
		BYTE2(0b11011110, 0b11111001), /*code*/
		2, 0, 0,      /*Size: opcode modeRm sib*/
		0,            /*immSize*/
		Code,         /*opcodeLastByte*/
		Null          /*modeRmType*/)