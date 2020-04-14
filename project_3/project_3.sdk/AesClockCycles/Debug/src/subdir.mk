################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/aes.c \
../src/aes_dec.c \
../src/aes_enc.c \
../src/aes_func.c \
../src/aes_key.c \
../src/aes_test.c \
../src/platform.c 

OBJS += \
./src/aes.o \
./src/aes_dec.o \
./src/aes_enc.o \
./src/aes_func.o \
./src/aes_key.o \
./src/aes_test.o \
./src/platform.o 

C_DEPS += \
./src/aes.d \
./src/aes_dec.d \
./src/aes_enc.d \
./src/aes_func.d \
./src/aes_key.d \
./src/aes_test.d \
./src/platform.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O3 -g3 -c -fmessage-length=0 -MT"$@" -I../../AesClockCycles_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


