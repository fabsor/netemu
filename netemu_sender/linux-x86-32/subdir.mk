################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kaillera_callbacks.c \
../netemu_communication.c \
../netemu_kaillera.c \
../netemu_p2p.c \
../netemu_resources.c \
../netemu_util.c \
../p2p_callbacks.c \
../responses.c 

OBJS += \
./kaillera_callbacks.o \
./netemu_communication.o \
./netemu_kaillera.o \
./netemu_p2p.o \
./netemu_resources.o \
./netemu_util.o \
./p2p_callbacks.o \
./responses.o 

C_DEPS += \
./kaillera_callbacks.d \
./netemu_communication.d \
./netemu_kaillera.d \
./netemu_p2p.d \
./netemu_resources.d \
./netemu_util.d \
./p2p_callbacks.d \
./responses.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I"/home/emil/workspace/netemu/netlib/headers" -O0 -g3 -pedantic -Wall -c -fmessage-length=0 -m32 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


