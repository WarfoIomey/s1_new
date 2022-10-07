################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/MQTT/Src/MQTTConnectClient.c \
../Middlewares/Third_Party/MQTT/Src/MQTTConnectServer.c \
../Middlewares/Third_Party/MQTT/Src/MQTTDeserializePublish.c \
../Middlewares/Third_Party/MQTT/Src/MQTTFormat.c \
../Middlewares/Third_Party/MQTT/Src/MQTTPacket.c \
../Middlewares/Third_Party/MQTT/Src/MQTTSerializePublish.c \
../Middlewares/Third_Party/MQTT/Src/MQTTSubscribeClient.c \
../Middlewares/Third_Party/MQTT/Src/MQTTSubscribeServer.c \
../Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeClient.c \
../Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeServer.c 

OBJS += \
./Middlewares/Third_Party/MQTT/Src/MQTTConnectClient.o \
./Middlewares/Third_Party/MQTT/Src/MQTTConnectServer.o \
./Middlewares/Third_Party/MQTT/Src/MQTTDeserializePublish.o \
./Middlewares/Third_Party/MQTT/Src/MQTTFormat.o \
./Middlewares/Third_Party/MQTT/Src/MQTTPacket.o \
./Middlewares/Third_Party/MQTT/Src/MQTTSerializePublish.o \
./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeClient.o \
./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeServer.o \
./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeClient.o \
./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeServer.o 

C_DEPS += \
./Middlewares/Third_Party/MQTT/Src/MQTTConnectClient.d \
./Middlewares/Third_Party/MQTT/Src/MQTTConnectServer.d \
./Middlewares/Third_Party/MQTT/Src/MQTTDeserializePublish.d \
./Middlewares/Third_Party/MQTT/Src/MQTTFormat.d \
./Middlewares/Third_Party/MQTT/Src/MQTTPacket.d \
./Middlewares/Third_Party/MQTT/Src/MQTTSerializePublish.d \
./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeClient.d \
./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeServer.d \
./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeClient.d \
./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeServer.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/MQTT/Src/%.o Middlewares/Third_Party/MQTT/Src/%.su: ../Middlewares/Third_Party/MQTT/Src/%.c Middlewares/Third_Party/MQTT/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/MQTT/Inc" -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/LwIP/src/include" -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/LwIP/system" -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/cJSON/Inc" -I"C:/Project/stm32f1rbt6/s1-main/Middlewares/Third_Party/cJSON/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-MQTT-2f-Src

clean-Middlewares-2f-Third_Party-2f-MQTT-2f-Src:
	-$(RM) ./Middlewares/Third_Party/MQTT/Src/MQTTConnectClient.d ./Middlewares/Third_Party/MQTT/Src/MQTTConnectClient.o ./Middlewares/Third_Party/MQTT/Src/MQTTConnectClient.su ./Middlewares/Third_Party/MQTT/Src/MQTTConnectServer.d ./Middlewares/Third_Party/MQTT/Src/MQTTConnectServer.o ./Middlewares/Third_Party/MQTT/Src/MQTTConnectServer.su ./Middlewares/Third_Party/MQTT/Src/MQTTDeserializePublish.d ./Middlewares/Third_Party/MQTT/Src/MQTTDeserializePublish.o ./Middlewares/Third_Party/MQTT/Src/MQTTDeserializePublish.su ./Middlewares/Third_Party/MQTT/Src/MQTTFormat.d ./Middlewares/Third_Party/MQTT/Src/MQTTFormat.o ./Middlewares/Third_Party/MQTT/Src/MQTTFormat.su ./Middlewares/Third_Party/MQTT/Src/MQTTPacket.d ./Middlewares/Third_Party/MQTT/Src/MQTTPacket.o ./Middlewares/Third_Party/MQTT/Src/MQTTPacket.su ./Middlewares/Third_Party/MQTT/Src/MQTTSerializePublish.d ./Middlewares/Third_Party/MQTT/Src/MQTTSerializePublish.o ./Middlewares/Third_Party/MQTT/Src/MQTTSerializePublish.su ./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeClient.d ./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeClient.o ./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeClient.su ./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeServer.d ./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeServer.o ./Middlewares/Third_Party/MQTT/Src/MQTTSubscribeServer.su ./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeClient.d ./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeClient.o ./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeClient.su ./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeServer.d ./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeServer.o ./Middlewares/Third_Party/MQTT/Src/MQTTUnsubscribeServer.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-MQTT-2f-Src

