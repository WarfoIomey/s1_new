################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Mqtt/Src/MQTTConnectClient.c \
../Mqtt/Src/MQTTConnectServer.c \
../Mqtt/Src/MQTTDeserializePublish.c \
../Mqtt/Src/MQTTFormat.c \
../Mqtt/Src/MQTTPacket.c \
../Mqtt/Src/MQTTSerializePublish.c \
../Mqtt/Src/MQTTSubscribeClient.c \
../Mqtt/Src/MQTTSubscribeServer.c \
../Mqtt/Src/MQTTUnsubscribeClient.c \
../Mqtt/Src/MQTTUnsubscribeServer.c 

OBJS += \
./Mqtt/Src/MQTTConnectClient.o \
./Mqtt/Src/MQTTConnectServer.o \
./Mqtt/Src/MQTTDeserializePublish.o \
./Mqtt/Src/MQTTFormat.o \
./Mqtt/Src/MQTTPacket.o \
./Mqtt/Src/MQTTSerializePublish.o \
./Mqtt/Src/MQTTSubscribeClient.o \
./Mqtt/Src/MQTTSubscribeServer.o \
./Mqtt/Src/MQTTUnsubscribeClient.o \
./Mqtt/Src/MQTTUnsubscribeServer.o 

C_DEPS += \
./Mqtt/Src/MQTTConnectClient.d \
./Mqtt/Src/MQTTConnectServer.d \
./Mqtt/Src/MQTTDeserializePublish.d \
./Mqtt/Src/MQTTFormat.d \
./Mqtt/Src/MQTTPacket.d \
./Mqtt/Src/MQTTSerializePublish.d \
./Mqtt/Src/MQTTSubscribeClient.d \
./Mqtt/Src/MQTTSubscribeServer.d \
./Mqtt/Src/MQTTUnsubscribeClient.d \
./Mqtt/Src/MQTTUnsubscribeServer.d 


# Each subdirectory must supply rules for building sources it contributes
Mqtt/Src/%.o Mqtt/Src/%.su: ../Mqtt/Src/%.c Mqtt/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Mqtt-2f-Src

clean-Mqtt-2f-Src:
	-$(RM) ./Mqtt/Src/MQTTConnectClient.d ./Mqtt/Src/MQTTConnectClient.o ./Mqtt/Src/MQTTConnectClient.su ./Mqtt/Src/MQTTConnectServer.d ./Mqtt/Src/MQTTConnectServer.o ./Mqtt/Src/MQTTConnectServer.su ./Mqtt/Src/MQTTDeserializePublish.d ./Mqtt/Src/MQTTDeserializePublish.o ./Mqtt/Src/MQTTDeserializePublish.su ./Mqtt/Src/MQTTFormat.d ./Mqtt/Src/MQTTFormat.o ./Mqtt/Src/MQTTFormat.su ./Mqtt/Src/MQTTPacket.d ./Mqtt/Src/MQTTPacket.o ./Mqtt/Src/MQTTPacket.su ./Mqtt/Src/MQTTSerializePublish.d ./Mqtt/Src/MQTTSerializePublish.o ./Mqtt/Src/MQTTSerializePublish.su ./Mqtt/Src/MQTTSubscribeClient.d ./Mqtt/Src/MQTTSubscribeClient.o ./Mqtt/Src/MQTTSubscribeClient.su ./Mqtt/Src/MQTTSubscribeServer.d ./Mqtt/Src/MQTTSubscribeServer.o ./Mqtt/Src/MQTTSubscribeServer.su ./Mqtt/Src/MQTTUnsubscribeClient.d ./Mqtt/Src/MQTTUnsubscribeClient.o ./Mqtt/Src/MQTTUnsubscribeClient.su ./Mqtt/Src/MQTTUnsubscribeServer.d ./Mqtt/Src/MQTTUnsubscribeServer.o ./Mqtt/Src/MQTTUnsubscribeServer.su

.PHONY: clean-Mqtt-2f-Src

