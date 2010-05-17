################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../host_test.c \
../join_test.c \
../kaillera_game_creator_test.c \
../kaillera_game_joiner_test.c \
../main.c \
../p2ptest.c 

OBJS += \
./host_test.o \
./join_test.o \
./kaillera_game_creator_test.o \
./kaillera_game_joiner_test.o \
./main.o \
./p2ptest.o 

C_DEPS += \
./host_test.d \
./join_test.d \
./kaillera_game_creator_test.d \
./kaillera_game_joiner_test.d \
./main.d \
./p2ptest.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I"/home/emil/workspace/netemu/netemu_sender/interface" -I"/home/emil/workspace/netemu/netlib/headers" -O0 -g3 -pg -p -pedantic -Wall -c -fmessage-length=0 -m32 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


