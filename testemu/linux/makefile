RM := rm -rf

NETEMU_DIR := ../../netemu/interface
NETLIB_DIR := ../../netlib/headers
NETEMU_LIB_DIR := ../../netemu/linux
NETLIB_LIB_DIR := ../../netlib/linux
NETLIB_PATH := ./build/libnetlib.a

-include testemu.mk

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: testemu

# Other Targets
clean:
	-$(RM) $(TESTEMU_OBJS)$(TESTEMU_C_DEPS)$(EXECUTABLES) testemu
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
