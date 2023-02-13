PROJECT_DIR := ./../..

include $(PROJECT_DIR)/makefile_settings

objs := $(addprefix $(OBJ)/, Lexer.o Identifier.o)
obj_lexer_test := $(OBJ)/LexerUnitTests.o

VPATH = ./Lexer

compile:
	@echo "\033[33mКомпиляция \033[1mLexer \033[22m...\033[0m"

	@$(MAKE) _compile --quiet

	@echo "\033[32mКомпиляция \033[1mLexer \033[22mуспешно завершена.\033[0m"

_compile: $(objs)

lexer_test: $(obj_lexer_test)

$(objs): $(OBJ)/%.o: %.cpp %.h
	@echo "\033[35m\tКомпиляция \033[1m$@ \033[22m...\033[0m"
	@$(COMP) -o $@ $< $(COMP_FLAGS)

$(obj_lexer_test): $(OBJ)/%.o: %.cpp
	@echo "\033[35m\tКомпиляция \033[1m$@ \033[22m...\033[0m"
	@$(COMP) -o $@ $< $(COMP_FLAGS)

$(OBJ)/Lexer.o: ../../Modules/Logs/Logs.h ../../Modules/ErrorsHandling.h ../../Modules/Text/Text.h \
			    ../../Modules/ExtArray/ExtArray.h ../../LanguageGrammar/LanguageGrammar.h Identifier.h
$(OBJ)/Identifier.o: ../../Modules/Logs/Logs.h ../../Modules/ErrorsHandling.h  ../../Modules/ExtArray/ExtArray.h \
					 ../../Modules/ExtHeap/ExtHeap.h ../../LanguageGrammar/LanguageGrammar.h Identifier.h
$(OBJ)/LexerUnitTests.o: ../../Modules/Logs/Logs.h ../../Modules/ErrorsHandling.h ../../Modules/Text/Text.h \
			             ../../Modules/ExtArray/ExtArray.h ../../LanguageGrammar/LanguageGrammar.h Identifier.h

.PHONY: _compile compile lexer_test
.DEFAULT_GOAL = compile