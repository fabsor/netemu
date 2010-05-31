# Add inputs and outputs from these tool invocations to the build variables 
NETEMU_C_SRCS += \
../resources.c \
../util.c 

NETEMU_C_OBJS += \
./resources.o \
./util.o 

NETEMU_C_DEPS += \
./resources.d \
./util.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_NIX -I$(NETLIB_DIR) -O3 -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


