################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tests/application_test.c \
../tests/communication_test.c \
../tests/network_test.c 

OBJS += \
./tests/application_test.o \
./tests/communication_test.o \
./tests/network_test.o 

C_DEPS += \
./tests/application_test.d \
./tests/communication_test.d \
./tests/network_test.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I"/home/fabian/workspace/netemu/netlib/headers" -O3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


