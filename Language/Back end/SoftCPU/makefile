PROJECT_DIR := ./../..

include $(PROJECT_DIR)/makefile_settings

objs := $(addprefix $(OBJ)/, SoftCpuCompiler.o)
obj_compiler_test := $(OBJ)/SoftCpuCompilerUnitTests.o

compile:
	@echo "\033[33mКомпиляция \033[1mBackend SoftCpu \033[22m...\033[0m"

	@$(MAKE) _compile --quiet

	@echo "\033[32mКомпиляция \033[1mBackend SoftCpu \033[22mуспешно завершена.\033[0m"

_compile: $(objs)

compiler_test: $(obj_compiler_test)

$(objs) $(obj_compiler_test): $(OBJ)/%.o: %.cpp
	@echo "\033[35m\tКомпиляция \033[1m$@ \033[22m...\033[0m"
	@$(COMP) -o $@ $< $(COMP_FLAGS)

.PHONY: compile _compile compiler_test
.DEFAULT_GOAL = compile