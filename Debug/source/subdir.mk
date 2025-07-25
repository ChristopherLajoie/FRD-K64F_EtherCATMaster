################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/CANopen_HAL.c \
../source/INIT_HAL.c \
../source/UART_HAL.c \
../source/Utilities.c \
../source/main.c \
../source/semihost_hardfault.c 

C_DEPS += \
./source/CANopen_HAL.d \
./source/INIT_HAL.d \
./source/UART_HAL.d \
./source/Utilities.d \
./source/main.d \
./source/semihost_hardfault.d 

OBJS += \
./source/CANopen_HAL.o \
./source/INIT_HAL.o \
./source/UART_HAL.o \
./source/Utilities.o \
./source/main.o \
./source/semihost_hardfault.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\header" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\board" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\utilities" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\drivers" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\device" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\component\serial_manager" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\component\lists" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\CMSIS" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\component\uart" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\mdio" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\phy" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/CANopen_HAL.d ./source/CANopen_HAL.o ./source/INIT_HAL.d ./source/INIT_HAL.o ./source/UART_HAL.d ./source/UART_HAL.o ./source/Utilities.d ./source/Utilities.o ./source/main.d ./source/main.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o

.PHONY: clean-source

