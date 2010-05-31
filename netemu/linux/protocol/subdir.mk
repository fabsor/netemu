# Add inputs and outputs from these tool invocations to the build variables 
NETEMU_C_SRCS += \
../protocol/application.c \
../protocol/application_kaillera.c \
../protocol/application_p2p.c \
../protocol/communication.c \
../protocol/transport.c 

NETEMU_OBJS += \
./protocol/application.o \
./protocol/application_kaillera.o \
./protocol/application_p2p.o \
./protocol/communication.o \
./protocol/transport.o 

NETEMU_C_DEPS += \
./protocol/application.d \
./protocol/application_kaillera.d \
./protocol/application_p2p.d \
./protocol/communication.d \
./protocol/transport.d 


# Each subdirectory must supply rules for building sources it contributes
protocol/%.o: ../protocol/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I$(NETLIB_DIR) -O3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


