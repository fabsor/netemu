################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../network/netemu_packet_buffer.c \
../network/netemu_receiver.c \
../network/netemu_sender_buffer.c \
../network/netemu_sender_collection.c \
../network/netemu_sender_udp.c \
../network/netemu_tcp.c \
../network/netemu_udp.c 

OBJS += \
./network/netemu_packet_buffer.o \
./network/netemu_receiver.o \
./network/netemu_sender_buffer.o \
./network/netemu_sender_collection.o \
./network/netemu_sender_udp.o \
./network/netemu_tcp.o \
./network/netemu_udp.o 

C_DEPS += \
./network/netemu_packet_buffer.d \
./network/netemu_receiver.d \
./network/netemu_sender_buffer.d \
./network/netemu_sender_collection.d \
./network/netemu_sender_udp.d \
./network/netemu_tcp.d \
./network/netemu_udp.d 


# Each subdirectory must supply rules for building sources it contributes
network/%.o: ../network/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I"/home/emil/workspace/netemu/netlib/headers" -O0 -g3 -pedantic -Wall -c -fmessage-length=0 -m32 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


