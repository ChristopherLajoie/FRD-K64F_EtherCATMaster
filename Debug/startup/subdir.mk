################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/startup_mk64f12.c 

C_DEPS += \
./startup/startup_mk64f12.d 

OBJS += \
./startup/startup_mk64f12.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.c startup/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\header" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\board" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\utilities" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\drivers" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\device" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\component\serial_manager" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\component\lists" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\CMSIS" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\component\uart" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\mdio" -I"C:\Users\foxtr\Documents\MCUXpressoIDE_25.6.136\workspace\MK64F12_Project\phy" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-startup

clean-startup:
	-$(RM) ./startup/startup_mk64f12.d ./startup/startup_mk64f12.o

.PHONY: clean-startup

