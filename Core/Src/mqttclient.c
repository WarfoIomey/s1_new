#include <mqttclient.h>

uint32_t mqtt_msgId = 0;

int mqtt_buflen = MQTT_PacketBuffSize;
uint8_t mqtt_PacketBuf[MQTT_PacketBuffSize];
/***********************ОТПРАВКА БУФЕР ПАКЕТА*****************************************/
int mqtt_transport_sendPacketBuffer(uint8_t *buf, int buflen) {

	// MQTT Head may have 0x00
	USART3_RecvEndFlag = 0;
	memset(USART3_RxBUF, 0, USART3_MAX_RECVLEN);
	HAL_UART_Transmit(&huart3, buf, buflen, 0xff);

	return buflen;
}
/***********************ОТПРАВКА БУФЕР ПАКЕТА*****************************************/
/***********************ОТПРАВКА ПОЛУЧЕННЫХ ДАННЫХ************************************/
int mqtt_transport_getdata(uint8_t *buf, int buflen) {

	if (MQTT_RecvEndFlag == 1) {
		memcpy(buf, (const char*) USART3_RxBUF, buflen);

		USART3_RxLen = 0;
		USART3_RecvEndFlag = 0;
		memset(USART3_RxBUF, 0, USART3_MAX_RECVLEN);
	}

	return buflen;
}
/***********************ОТПРАВКА ПОЛУЧЕННЫХ ДАННЫХ************************************/
/***************************ОТПРАВКА ОТКРЫТА******************************************/
int mqtt_transport_open(uint8_t *host, int port) {
	return 1;
}
/***************************ОТПРАВКА ОТКРЫТА******************************************/
/***************************ОТПРАВКА ЗАКРЫТА******************************************/
int mqtt_transport_close(int sock) {
	return 0;
}
/***************************ОТПРАВКА ЗАКРРЫТА******************************************/
uint8_t mqtt_ConnectServer() {

	uint8_t responMsg = 0xff;
	uint8_t sessionPresent = 0;
	uint8_t connack_rc = 0;

	mqtt_buflen = sizeof(mqtt_PacketBuf);

	MQTTPacket_connectData ConnectData = MQTTPacket_connectData_initializer;
	ConnectData.clientID.cstring = MQTT_CLIENTID;
	ConnectData.username.cstring = MQTT_USERNAME;
	ConnectData.password.cstring = MQTT_PASSWORD;
	ConnectData.keepAliveInterval = MQTT_KeepAliveInterval;
	ConnectData.MQTTVersion = 4;
	ConnectData.cleansession = 1;

	u2_printf("(DBG:) ПОПЫТКА ПОДКЛЮЧИТЬСЯ MQTT СЕРВЕРУ\r\n\n");
	printf("(DBG:) ПОПЫТКА ПОДКЛЮЧИТЬСЯ MQTT СЕРВЕРУ\r\n\n");


	// ИСПОЛЬЗОВАНИЕ CONNACK В КАЧЕСТВЕ ДЛИННЫ
	connack_rc = MQTTSerialize_connect(mqtt_PacketBuf, mqtt_buflen,
			&ConnectData); // СБОРКА ПАКЕТА ПОДКЛЮЧЕНИЯ
	mqtt_transport_sendPacketBuffer(mqtt_PacketBuf, connack_rc);

	do {
		while (responMsg != CONNACK) {
			responMsg = MQTTPacket_read(mqtt_PacketBuf, mqtt_buflen,
					mqtt_transport_getdata);
		}

		HAL_UART_Receive_DMA(&huart3, USART3_RxBUF, USART3_MAX_RECVLEN);

	} while (MQTTDeserialize_connack(&sessionPresent, &connack_rc,
			mqtt_PacketBuf, mqtt_buflen) != 1 || connack_rc != 0);

	if (connack_rc != 0) {
		u2_printf("connack_rc:%uc\r\n", connack_rc);
		printf("connack_rc:%uc\r\n", connack_rc);

	}

	u2_printf("CONNECT OK!\r\n");
	printf("CONNECT OK!\r\n");


	HAL_Delay(3000);
	connack_rc = MQTTSerialize_disconnect(mqtt_PacketBuf, mqtt_buflen);
	mqtt_transport_sendPacketBuffer(mqtt_PacketBuf, connack_rc);

	return 1;
}
