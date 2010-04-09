################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../netemu_list.c \
../netemu_socket_nix.c \
../netemu_thread_nix.c \
../netemu_util_nix.c 

OBJS += \
./netemu_list.o \
./netemu_socket_nix.o \
./netemu_thread_nix.o \
./netemu_util_nix.o 

C_DEPS += \
./netemu_list.d \
./netemu_socket_nix.d \
./netemu_thread_nix.d \
./netemu_util_nix.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I"/home/fabbe/workspace/netemu/netlib/headers" -O0 -g3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


