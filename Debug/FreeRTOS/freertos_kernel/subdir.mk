################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS/freertos_kernel/croutine.c \
../FreeRTOS/freertos_kernel/event_groups.c \
../FreeRTOS/freertos_kernel/list.c \
../FreeRTOS/freertos_kernel/queue.c \
../FreeRTOS/freertos_kernel/stream_buffer.c \
../FreeRTOS/freertos_kernel/tasks.c \
../FreeRTOS/freertos_kernel/timers.c 

C_DEPS += \
./FreeRTOS/freertos_kernel/croutine.d \
./FreeRTOS/freertos_kernel/event_groups.d \
./FreeRTOS/freertos_kernel/list.d \
./FreeRTOS/freertos_kernel/queue.d \
./FreeRTOS/freertos_kernel/stream_buffer.d \
./FreeRTOS/freertos_kernel/tasks.d \
./FreeRTOS/freertos_kernel/timers.d 

OBJS += \
./FreeRTOS/freertos_kernel/croutine.o \
./FreeRTOS/freertos_kernel/event_groups.o \
./FreeRTOS/freertos_kernel/list.o \
./FreeRTOS/freertos_kernel/queue.o \
./FreeRTOS/freertos_kernel/stream_buffer.o \
./FreeRTOS/freertos_kernel/tasks.o \
./FreeRTOS/freertos_kernel/timers.o 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/freertos_kernel/%.o: ../FreeRTOS/freertos_kernel/%.c FreeRTOS/freertos_kernel/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\board" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\utilities" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\drivers" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\device" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\component\serial_manager" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\component\lists" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\CMSIS" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\component\uart" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\mdio" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\phy" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\source" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\header" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\FreeRTOS\freertos_kernel\include" -I"C:\MCUXpressoIDE_25.6.136\workspace\EtherCATMaster\FreeRTOS\freertos_kernel\portable\GCC\ARM_CM4F" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-FreeRTOS-2f-freertos_kernel

clean-FreeRTOS-2f-freertos_kernel:
	-$(RM) ./FreeRTOS/freertos_kernel/croutine.d ./FreeRTOS/freertos_kernel/croutine.o ./FreeRTOS/freertos_kernel/event_groups.d ./FreeRTOS/freertos_kernel/event_groups.o ./FreeRTOS/freertos_kernel/list.d ./FreeRTOS/freertos_kernel/list.o ./FreeRTOS/freertos_kernel/queue.d ./FreeRTOS/freertos_kernel/queue.o ./FreeRTOS/freertos_kernel/stream_buffer.d ./FreeRTOS/freertos_kernel/stream_buffer.o ./FreeRTOS/freertos_kernel/tasks.d ./FreeRTOS/freertos_kernel/tasks.o ./FreeRTOS/freertos_kernel/timers.d ./FreeRTOS/freertos_kernel/timers.o

.PHONY: clean-FreeRTOS-2f-freertos_kernel

