################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
NETEMU_C_SRCS += \
../structures/netemu_hashtbl.c \
../structures/netemu_list.c \
../structures/netemu_stringbuilder.c 

NETEMU_OBJS += \
./structures/netemu_hashtbl.o \
./structures/netemu_list.o \
./structures/netemu_stringbuilder.o 

NETEMU_C_DEPS += \
./structures/netemu_hashtbl.d \
./structures/netemu_list.d \
./structures/netemu_stringbuilder.d 


# Each subdirectory must supply rules for building sources it contributes
structures/%.o: ../structures/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I$(NETLIB_DIR) -O3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


