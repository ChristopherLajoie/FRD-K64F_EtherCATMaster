################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_adc16.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_enet.c \
../drivers/fsl_flexcan.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_smc.c \
../drivers/fsl_uart.c 

C_DEPS += \
./drivers/fsl_adc16.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_enet.d \
./drivers/fsl_flexcan.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_smc.d \
./drivers/fsl_uart.d 

OBJS += \
./drivers/fsl_adc16.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_enet.o \
./drivers/fsl_flexcan.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_smc.o \
./drivers/fsl_uart.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\header" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\board" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\utilities" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\drivers" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\device" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\component\serial_manager" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\component\lists" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\CMSIS" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\component\uart" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\mdio" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\phy" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/fsl_adc16.d ./drivers/fsl_adc16.o ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_common_arm.d ./drivers/fsl_common_arm.o ./drivers/fsl_enet.d ./drivers/fsl_enet.o ./drivers/fsl_flexcan.d ./drivers/fsl_flexcan.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_i2c.d ./drivers/fsl_i2c.o ./drivers/fsl_smc.d ./drivers/fsl_smc.o ./drivers/fsl_uart.d ./drivers/fsl_uart.o

.PHONY: clean-drivers

