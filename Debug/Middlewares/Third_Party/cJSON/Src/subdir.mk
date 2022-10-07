################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/cJSON/Src/cJSON.c 

OBJS += \
./Middlewares/Third_Party/cJSON/Src/cJSON.o 

C_DEPS += \
./Middlewares/Third_Party/cJSON/Src/cJSON.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/cJSON/Src/%.o Middlewares/Third_Party/cJSON/Src/%.su: ../Middlewares/Third_Party/cJSON/Src/%.c Middlewares/Third_Party/cJSON/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/MQTT/Inc" -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/LwIP/src/include" -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/LwIP/system" -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/cJSON/Inc" -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/cJSON/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-cJSON-2f-Src

clean-Middlewares-2f-Third_Party-2f-cJSON-2f-Src:
	-$(RM) ./Middlewares/Third_Party/cJSON/Src/cJSON.d ./Middlewares/Third_Party/cJSON/Src/cJSON.o ./Middlewares/Third_Party/cJSON/Src/cJSON.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-cJSON-2f-Src

