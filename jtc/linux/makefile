-include ../../Makefile.rules

INCLUDE_DIRS := ../include
OUT_LIB := libjtc.so

# All of the sources participating in the build are defined here
-include objects.mk
DEPS := $(OBJS:%.o=%.d)
-include $(DEPS)

# All Target
all: $(OUT_LIB)

%.o: ../src/%.cpp
	@echo 'Building file: ../src/$@'
	@echo 'Invoking: GCC C++ Compiler'
	$(CPP_COMPILER) $(INCLUDE_DIRS:%=-I"%") -O0 -g3 -Wall -c -fPIC -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "../src/$(@:%.o=%.cpp)"
	@echo 'Finished building: $@'
	@echo 

$(OUT_LIB): $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CPP_LINKER) -shared -o"$(OUT_LIB)" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
install:
	@$(CP) $(OUT_LIB) $(INSTALL_DIR)

clean:
	-$(RM) $(OBJS) $(OBJS:%.o=%.d) $(OUT_LIB)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

