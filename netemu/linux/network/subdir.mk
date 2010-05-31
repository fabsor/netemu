################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../network/receiver_buffer.c \
../network/receiver_udp.c \
../network/sender_buffer.c \
../network/sender_collection.c \
../network/sender_udp.c \
../network/tcp.c 

OBJS += \
./network/receiver_buffer.o \
./network/receiver_udp.o \
./network/sender_buffer.o \
./network/sender_collection.o \
./network/sender_udp.o \
./network/tcp.o 

C_DEPS += \
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
	gcc -D_NIX -I"/home/fabbe/workspace/netemu/netlib/headers" -O3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


