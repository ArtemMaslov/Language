PROJECT_DIR := ./..

include $(PROJECT_DIR)/makefile_settings

objs := $(addprefix $(OBJ)/, LanguageGrammar.o)

compile:
	@echo "\033[33mКомпиляция \033[1mLanguageGrammar \033[22m...\033[0m"

	@$(MAKE) _compile --quiet

	@echo "\033[32mКомпиляция \033[1mLanguageGrammar \033[22mуспешно завершена.\033[0m"

_compile: $(objs)

$(objs): $(OBJ)/%.o: %.cpp %.h
	@echo "\033[35m\tКомпиляция \033[1m$@ \033[22m...\033[0m"
	@$(COMP) -o $@ $< $(COMP_FLAGS)

$(OBJ)/LanguageGrammar.o: keywords.inc operators.inc special_symbols.inc English/keywords.inc \
						  Universal/operators.inc Universal/special_symbols.inc

.PHONY: compile _compile
.DEFAULT_GOAL = compile