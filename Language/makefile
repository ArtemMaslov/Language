PROJECT_DIR := .

include $(PROJECT_DIR)/makefile_settings
include $(PROJECT_DIR)/makefile_utils

objs_grammar  := LanguageGrammar.o
objs_lexer    := Identifier.o Lexer.o
objs_parser   := parse_aux.o parse_expr.o parse_funct.o parse_keyword.o Parser.o
objs_modules  := Text.o Logs.o ExtArray.o ExtHeap.o FilesIO.o
objs_ast      := aux_nodes.o expr_nodes.o funct_nodes.o keyword_nodes.o AST.o AST_Verify.o
objs_x86      := x86Compiler.o asm_representation.o asm_labels.o \
			     compile_aux.o compile_expr.o compile_funct.o compile_keywords.o
objs_soft_cpu := SoftCpuCompiler.o

objs_frontend := $(objs_lexer) $(objs_parser) $(objs_grammar)

$(OBJ)/main.o: main.cpp
	@echo Компиляция $@
	@$(COMP) -o $(OBJ64)/$@ $< $(COMP_FLAGS)

compile: dir_obj compile_modules compile_ast compile_frontend compile_x86 compile_soft_cpu

lexer_test: dir_obj dir_bin compile_modules compile_grammar compile_lexer
	@cd ./Front\ end/Lexer && $(MAKE) lexer_test --no-print-directory --quiet
	@echo "\033[33mРедактирование связей ...\033[0m"

	@$(COMP) -o $(TARGET_PATH) $(addprefix $(OBJ)/, \
	$(objs_modules) $(objs_grammar) $(objs_lexer) LexerUnitTests.o) $(LINK_FLAGS)

	@echo "\033[32mСборка программы успешно завершена.\033[0m"

parser_test: dir_obj dir_bin compile_modules compile_frontend compile_ast
	@cd ./Front\ end/ && $(MAKE) parser_test --no-print-directory --quiet
	@echo "\033[33mРедактирование связей ...\033[0m"

	@$(COMP) -o $(TARGET_PATH) $(addprefix $(OBJ)/, \
	$(objs_modules) $(objs_ast) $(objs_frontend) ParserUnitTests.o) $(LINK_FLAGS)

	@echo "\033[32mСборка программы успешно завершена.\033[0m"

x86_test: dir_obj dir_bin compile_modules compile_frontend compile_ast compile_x86
	@cd ./Back\ end/x86 && $(MAKE) compiler_test --no-print-directory --quiet
	@echo "\033[33mРедактирование связей ...\033[0m"

	@$(COMP) -o $(TARGET_PATH) $(addprefix $(OBJ)/, \
	$(objs_modules) $(objs_ast) $(objs_frontend) $(objs_x86) x86CompilerUnitTests.o) $(LINK_FLAGS)

	@echo "\033[32mСборка программы успешно завершена.\033[0m"

soft_cpu_test: dir_obj dir_bin compile_modules compile_frontend compile_ast compile_soft_cpu
	@cd ./Back\ end/SoftCPU && $(MAKE) compiler_test --no-print-directory --quiet
	@echo "\033[33mРедактирование связей ...\033[0m"

	@$(COMP) -o $(TARGET_PATH) $(addprefix $(OBJ)/, \
	$(objs_modules) $(objs_ast) $(objs_frontend) $(objs_soft_cpu) SoftCpuCompilerUnitTests.o) $(LINK_FLAGS)

	@echo "\033[32mСборка программы успешно завершена.\033[0m"

default: compile

compile_frontend: compile_grammar compile_lexer compile_parser

compile_modules:
	@cd ./Modules && $(MAKE) --no-print-directory
compile_grammar:
	@cd ./LanguageGrammar && $(MAKE) --no-print-directory
compile_ast:
	@cd ./AST && $(MAKE) --no-print-directory
compile_lexer:
	@cd ./Front\ end/Lexer && $(MAKE) --no-print-directory
compile_parser:
	@cd ./Front\ end && $(MAKE) --no-print-directory
compile_x86:
	@cd ./Back\ end/x86 && $(MAKE) --no-print-directory
compile_soft_cpu:
	@cd ./Back\ end/SoftCPU && $(MAKE) --no-print-directory

.PHONY: compile compile_modules compile_grammar compile_ast compile_lexer compile_parser \
		compile_frontend compile_x86 compile_soft_cpu \
		x86_test soft_cpu_test parser_test lexer_test

.DEFAULT_GOAL = default