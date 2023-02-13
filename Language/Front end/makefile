PROJECT_DIR := ./..

include $(PROJECT_DIR)/makefile_settings

obj_parser_test := $(OBJ)/ParserUnitTests.o
objs := $(addprefix $(OBJ)/, Parser.o parse_aux.o \
							 parse_expr.o parse_funct.o parse_keyword.o)

compile:
	@echo "\033[33mКомпиляция \033[1mParser \033[22m...\033[0m"
	@$(MAKE) _compile --quiet
	@echo "\033[32mКомпиляция \033[1mParser \033[22mуспешно завершена.\033[0m"

_compile: $(objs)

parser_test: $(obj_parser_test)

$(objs) $(obj_parser_test): $(OBJ)/%.o: %.cpp
	@echo "\033[35mКомпиляция \033[1m$@ \033[22m...\033[0m"
	@$(COMP) -o $@ $< $(COMP_FLAGS)

$(OBJ)/Lexer.o: ../Modules/Logs/Logs.h ../Modules/ErrorsHandling.h ../Modules/Text/Text.h \
			    ../Modules/ExtArray/ExtArray.h ../LanguageGrammar/LanguageGrammar.h Identifier.h
$(OBJ)/Identifier.o: ../Modules/Logs/Logs.h ../Modules/ErrorsHandling.h  ../Modules/ExtArray/ExtArray.h \
					 ../Modules/ExtHeap/ExtHeap.h ../LanguageGrammar/LanguageGrammar.h Identifier.h

$(objs_parser): ../Modules/Logs/Logs.h ../Modules/ErrorsHandling.h Parser.h parser_private.h \
			    ../Modules/ExtArray/ExtArray.h ../AST/AST.h Lexer/Lexer.h

.PHONY: compile parser_test compile_parser
.DEFAULT_GOAL = compile