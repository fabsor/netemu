# All of the sources participating in the build are defined here
-include sourcefiles.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(NETLIB_C_DEPS)),)
-include $(NETLIB_C_DEPS)
endif
endif

# Tool invocations
libnetlib.a: $(NETLIB_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r $(NETLIB_PATH) $(NETLIB_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	-$(RM) $(NETLIB_OBJS)$(NETLIB_C_DEPS)
