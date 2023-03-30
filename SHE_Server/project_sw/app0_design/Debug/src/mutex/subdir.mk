################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mutex/xmutex.c 

OBJS += \
./src/mutex/xmutex.o 

C_DEPS += \
./src/mutex/xmutex.d 


# Each subdirectory must supply rules for building sources it contributes
src/mutex/%.o: ../src/mutex/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -I../../app0_bsp/psu_cortexa53_1/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


