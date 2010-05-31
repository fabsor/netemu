RM := rm -rf
NETLIB_DIR := ../../netlib/headers

SUBDIRS := \
. \
structures \
protocol \
p2p \
network \
kaillera \

# All of the sources participating in the build are defined here
-include subdir.mk
-include structures/subdir.mk
-include protocol/subdir.mk
-include p2p/subdir.mk
-include network/subdir.mk
-include kaillera/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(NETEMU_C_DEPS)),)
-include $(NETEMU_C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libnetemu.a

# Tool invocations
libnetemu.a: $(NETEMU_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r "libnetemu.a" $(NETEMU_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	-$(RM) $(NETEMU_OBJS)$(NETEMU_C_DEPS)

# Other Targets
clean:
	-$(RM) $(NETEMU_OBJS)$(NETEMU_C_DEPS)$(ARCHIVES) libnetemu.a
	-@echo ' '
