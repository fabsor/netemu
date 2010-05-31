# All of the sources participating in the build are defined here
SUBDIRS := \
. \

TESTEMU_LIBS := -lnetemu -lnetlib -lpthread
-include subdir.mk


ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(TESTEMU_C_DEPS)),)
-include $(TESTEMU_C_DEPS)
endif
endif

# Tool invocations
testemu: $(TESTEMU_OBJS) $(TESTEMU_USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L$(NETEMU_LIB_DIR) -L$(NETLIB_LIB_DIR) -pg -p -o"testemu" $(TESTEMU_OBJS) $(TESTEMU_LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
