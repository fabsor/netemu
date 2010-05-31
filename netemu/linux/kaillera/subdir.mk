################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
NETEMU_C_SRCS += \
../kaillera/kaillera.c \
../kaillera/kaillera_callbacks.c \
../kaillera/kaillera_communication.c \
../kaillera/kaillera_responses.c 

NETEMU_OBJS += \
./kaillera/kaillera.o \
./kaillera/kaillera_callbacks.o \
./kaillera/kaillera_communication.o \
./kaillera/kaillera_responses.o 

NETEMU_C_DEPS += \
./kaillera/kaillera.d \
./kaillera/kaillera_callbacks.d \
./kaillera/kaillera_communication.d \
./kaillera/kaillera_responses.d 


# Each subdirectory must supply rules for building sources it contributes
kaillera/%.o: ../kaillera/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I$(NETLIB_DIR) -O3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


