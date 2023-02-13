PROJECT_DIR := ./..

include $(PROJECT_DIR)/makefile_settings

objs := $(addprefix $(OBJ)/, Text.o Logs.o FilesIO.o ExtHeap.o ExtArray.o)

VPATH = ./Text ./Logs ./FilesIO ./ExtHeap ./ExtArray

compile:
	@echo "\033[33mКомпиляция \033[1mModules \033[22m...\033[0m"

	@$(MAKE) _compile --quiet

	@echo "\033[32mКомпиляция \033[1mModules\033[22m успешно завершена.\033[0m"

_compile: $(objs)

$(objs): $(OBJ)/%.o: %.cpp %.h
	@echo "\033[35m\tКомпиляция \033[1m$@ \033[22m...\033[0m"
	@$(COMP) -o $@ $< $(COMP_FLAGS)

$(OBJ)/Text.o: FilesIO.h ErrorsHandling.h Logs.h DisableLogs.h
$(OBJ)/Logs.o: TargetOS.h Logs.h logs_config_private.h
$(OBJ)/FilesIO.o: TargetOS.h Logs.h DisableLogs.h
$(OBJ)/ExtHeap.o: TargetOS.h ErrorsHandling.h Logs.h DisableLogs.h
$(OBJ)/ExtArray.o: TargetOS.h ErrorsHandling.h Logs.h DisableLogs.h

.PHONY: compile _compile
.DEFAULT_GOAL = compile