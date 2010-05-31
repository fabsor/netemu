# Add inputs and outputs from these tool invocations to the build variables 
NETLIB_C_SRCS += \
../netlib_error_nix.c \
../netlib_network_nix.c \
../netlib_thread_nix.c \
../netlib_util_nix.c 

NETLIB_OBJS += \
./netlib_error_nix.o \
./netlib_network_nix.o \
./netlib_thread_nix.o \
./netlib_util_nix.o 

NETLIB_C_DEPS += \
./netlib_error_nix.d \
./netlib_network_nix.d \
./netlib_thread_nix.d \
./netlib_util_nix.d 

CC = gcc
OPTIMIZE = -O3

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX $(OPTIMIZE) -pedantic -Wall -c -fmessage-length=0 -CB -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


