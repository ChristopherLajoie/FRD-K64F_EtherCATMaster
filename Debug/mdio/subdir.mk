################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../mdio/fsl_enet_mdio.c 

C_DEPS += \
./mdio/fsl_enet_mdio.d 

OBJS += \
./mdio/fsl_enet_mdio.o 


# Each subdirectory must supply rules for building sources it contributes
mdio/%.o: ../mdio/%.c mdio/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\board" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\utilities" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\drivers" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\device" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\component\serial_manager" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\component\lists" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\CMSIS" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\component\uart" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\mdio" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\phy" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\source" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\header" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\FreeRTOS\freertos_kernel\include" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\FreeRTOS\freertos_kernel\portable\GCC\ARM_CM4F" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-mdio

clean-mdio:
	-$(RM) ./mdio/fsl_enet_mdio.d ./mdio/fsl_enet_mdio.o

.PHONY: clean-mdio

