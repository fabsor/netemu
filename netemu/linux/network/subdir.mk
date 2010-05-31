################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
NETEMU_C_SRCS += \
../network/receiver_buffer.c \
../network/receiver_udp.c \
../network/sender_buffer.c \
../network/sender_collection.c \
../network/sender_udp.c \
../network/tcp.c 

NETEMU_OBJS += \
./network/receiver_buffer.o \
./network/receiver_udp.o \
./network/sender_buffer.o \
./network/sender_collection.o \
./network/sender_udp.o \
./network/tcp.o 

NETEMU_C_DEPS += \
./network/receiver_buffer.d \
./network/receiver_udp.d \
./network/sender_buffer.d \
./network/sender_collection.d \
./network/sender_udp.d \
./network/tcp.d 


# Each subdirectory must supply rules for building sources it contributes
network/%.o: ../network/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I$(NETLIB_DIR) -O3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


