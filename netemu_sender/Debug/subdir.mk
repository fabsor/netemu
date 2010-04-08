################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../application_test.c \
../communication_test.c \
../main.c \
../network_test.c \
../test_util.c 

OBJS += \
./application_test.o \
./communication_test.o \
./main.o \
./network_test.o \
./test_util.o 

C_DEPS += \
./application_test.d \
./communication_test.d \
./main.d \
./network_test.d \
./test_util.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I"/home/fabian/workspace/netemu/netlib/headers" -O0 -g3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


