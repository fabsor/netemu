################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../p2p/netemu_p2p.c \
../p2p/p2p_callbacks.c \
../p2p/p2p_responses.c 

OBJS += \
./p2p/netemu_p2p.o \
./p2p/p2p_callbacks.o \
./p2p/p2p_responses.o 

C_DEPS += \
./p2p/netemu_p2p.d \
./p2p/p2p_callbacks.d \
./p2p/p2p_responses.d 


# Each subdirectory must supply rules for building sources it contributes
p2p/%.o: ../p2p/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I"/home/fabbe/workspace/netemu/netlib/headers" -O3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


