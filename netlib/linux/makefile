# This makefile creates a static library out of netlib.

RM := rm -rf
NETLIB_PATH := ./netlib.a
SUBDIRS := \
. \

-include netlib.mk

# All Target
all: libnetlib.a

# Other Targets
clean:
	-$(RM) $(NETLIB_OBJS)$(NETLIB_C_DEPS)$(ARCHIVES) libnetlib.a
	-@echo ' '
