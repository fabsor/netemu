################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../network/netemu_receiver.c \
../network/netemu_sender.c 

OBJS += \
./network/netemu_receiver.o \
./network/netemu_sender.o 

C_DEPS += \
./network/netemu_receiver.d \
./network/netemu_sender.d 


# Each subdirectory must supply rules for building sources it contributes
network/%.o: ../network/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I"/home/fabian/workspace/netemu/netlib/headers" -O0 -g3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


