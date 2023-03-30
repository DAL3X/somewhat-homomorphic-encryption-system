################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/code.c \
../src/hardware.c \
../src/homenc.c \
../src/main.c \
../src/platform.c 

OBJS += \
./src/code.o \
./src/hardware.o \
./src/homenc.o \
./src/main.o \
./src/platform.o 

C_DEPS += \
./src/code.d \
./src/hardware.d \
./src/homenc.d \
./src/main.d \
./src/platform.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -I../../app1_bsp/psu_cortexa53_2/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


