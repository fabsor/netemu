################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
TESTEMU_C_SRCS += \
../kaillera_game_creator_test.c \
../kaillera_game_joiner_test.c \
../main.c \
../p2p_build_network_test.c \
../p2p_host_test.c \
../p2p_join_test.c \
../p2ptest.c 

TESTEMU_OBJS += \
./kaillera_game_creator_test.o \
./kaillera_game_joiner_test.o \
./main.o \
./p2p_build_network_test.o \
./p2p_host_test.o \
./p2p_join_test.o \
./p2ptest.o 

TESTEMU_C_DEPS += \
./kaillera_game_creator_test.d \
./kaillera_game_joiner_test.d \
./main.d \
./p2p_build_network_test.d \
./p2p_host_test.d \
./p2p_join_test.d \
./p2ptest.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I$(NETEMU_DIR) -I$(NETLIB_DIR) -O3 -pg -p -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


