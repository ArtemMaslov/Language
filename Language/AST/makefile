PROJECT_DIR := ./..

include $(PROJECT_DIR)/makefile_settings

objs := $(addprefix $(OBJ)/, aux_nodes.o expr_nodes.o funct_nodes.o keyword_nodes.o AST.o AST_Verify.o)

compile:
	@echo "\033[33mКомпиляция \033[1mAST \033[22m...\033[0m"

	@$(MAKE) _compile --quiet

	@echo "\033[32mКомпиляция \033[1mAST \033[22mуспешно завершена.\033[0m"

_compile: $(objs)

$(objs): $(OBJ)/%.o: %.cpp
	@echo "\033[35mКомпиляция \033[1m$@ \033[22m...\033[0m"
	@$(COMP) -o $@ $< $(COMP_FLAGS)

.PHONY: compile _compile
.DEFAULT_GOAL = compile