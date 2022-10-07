/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "MQTTPacket.h"
#include "StackTrace.h"

#include <string.h>

/**
* Определяет длину пакета отмены подписки MQTT, который будет создан с использованием предоставленных параметров
  * @* @param подсчитывает количество строк фильтра темы в фильтрах темы
  * @* @param тема Фильтрует массив строк фильтра темы, которые будут использоваться в публикации
  * @возвращает длину буфера, необходимую для хранения сериализованной версии пакета
  */
int MQTTSerialize_unsubscribeLength(int count, MQTTString topicFilters[])
{
	int i;
	int len = 2; /* packetid */

	for (i = 0; i < count; ++i)
		len += 2 + MQTTstrlen(topicFilters[i]); /* length + topic*/
	return len;
}


/**
* Сериализует предоставленные данные об отказе от подписки в предоставленный буфер, готовый к отправке
  * @param buf необработанные данные буфера правильной длины, определяемой полем оставшаяся длина
  * @param buflen длина в байтах данных в предоставленном буфере
  * @param dup integer - флаг MQTT dup
  * @param packetid целое число - идентификатор пакета MQTT
  * @* @param count - количество участников в массиве фильтров тем.
  * @* @param Фильтры тем - массив имен фильтров тем
  * @возвращает длину сериализованных данных.  <= 0 указывает на ошибку
  */
int MQTTSerialize_unsubscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid,
		int count, MQTTString topicFilters[])
{
	unsigned char *ptr = buf;
	MQTTHeader header = {0};
	int rem_len = 0;
	int rc = -1;
	int i = 0;

	FUNC_ENTRY;
	if (MQTTPacket_len(rem_len = MQTTSerialize_unsubscribeLength(count, topicFilters)) > buflen)
	{
		rc = MQTTPACKET_BUFFER_TOO_SHORT;
		goto exit;
	}

	header.byte = 0;
	header.bits.type = UNSUBSCRIBE;
	header.bits.dup = dup;
	header.bits.qos = 1;
	writeChar(&ptr, header.byte);

	ptr += MQTTPacket_encode(ptr, rem_len);

	writeInt(&ptr, packetid);

	for (i = 0; i < count; ++i)
		writeMQTTString(&ptr, topicFilters[i]);

	rc = ptr - buf;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
* Десериализует поставляемый (проводной) буфер в неподтвержденные данные
  * @param packetid возвращаемое целое число - идентификатор пакета MQTT
  * @param buf необработанные данные буфера правильной длины, определяемой полем оставшаяся длина
  * @param buflen длина в байтах данных в предоставленном буфере
  * @возвращает код ошибки.  1 - это успех, 0 - неудача
  */
int MQTTDeserialize_unsuback(unsigned short* packetid, unsigned char* buf, int buflen)
{
	unsigned char type = 0;
	unsigned char dup = 0;
	int rc = 0;

	FUNC_ENTRY;
	rc = MQTTDeserialize_ack(&type, &dup, packetid, buf, buflen);
	if (type == UNSUBACK)
		rc = 1;
	FUNC_EXIT_RC(rc);
	return rc;
}


