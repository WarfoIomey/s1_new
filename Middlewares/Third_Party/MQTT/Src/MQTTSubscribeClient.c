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
* Определяет длину пакета подписки MQTT, который будет создан с использованием предоставленных параметров
  * @* @param подсчитывает количество строк фильтра темы в фильтрах темы
  * @* @param тема Фильтрует массив строк фильтра темы, которые будут использоваться в публикации
  * @возвращает длину буфера, необходимую для хранения сериализованной версии пакета
  */
int MQTTSerialize_subscribeLength(int count, MQTTString topicFilters[])
{
	int i;
	int len = 2; /* packetid */

	for (i = 0; i < count; ++i)
		len += 2 + MQTTstrlen(topicFilters[i]) + 1;
	return len;
}


/**
* Сериализует предоставленные данные подписки в предоставленный буфер, готовый к отправке
  * @param buf буфер, в который будет сериализован пакет
  * @* @param buflen длина в байтах предоставленного буфера
  * @param dup integer - флаг MQTT dup
  * @param packetid целое число - идентификатор пакета MQTT
  * @* @param count - количество участников в фильтрах тем и массивах запросов Qos
  * @* @param Фильтры тем - массив имен фильтров тем
  * @param requestedQoSs - массив запрошенных QoS
  * @возвращает длину сериализованных данных.  <= 0 указывает на ошибку
  */
int MQTTSerialize_subscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid, int count,
		MQTTString topicFilters[], int requestedQoSs[])
{
	unsigned char *ptr = buf;
	MQTTHeader header = {0};
	int rem_len = 0;
	int rc = 0;
	int i = 0;

	FUNC_ENTRY;
	if (MQTTPacket_len(rem_len = MQTTSerialize_subscribeLength(count, topicFilters)) > buflen)
	{
		rc = MQTTPACKET_BUFFER_TOO_SHORT;
		goto exit;
	}

	header.byte = 0;
	header.bits.type = SUBSCRIBE;
	header.bits.dup = dup;
	header.bits.qos = 1;
	writeChar(&ptr, header.byte);

	ptr += MQTTPacket_encode(ptr, rem_len);

	writeInt(&ptr, packetid);

	for (i = 0; i < count; ++i)
	{
		writeMQTTString(&ptr, topicFilters[i]);
		writeChar(&ptr, requestedQoSs[i]);
	}

	rc = ptr - buf;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}



/**
  * * Десериализует поставляемый (проводной) буфер в данные подпакета
  * @param packetid возвращаемое целое число - идентификатор пакета MQTT
  * @param maxcount - максимальное количество элементов, разрешенных в массиве grantedQoSs
  * @param count возвращает целое число - количество элементов в массиве grantedQoSs
  * @param grantedQoSs возвращает массив целых чисел - предоставленные качества обслуживания
  * @param buf необработанные данные буфера правильной длины, определяемой полем оставшаяся длина
  * @param buflen длина в байтах данных в предоставленном буфере
  * @возвращает код ошибки.  1 - это успех, 0 - неудача
  */
int MQTTDeserialize_suback(unsigned short* packetid, int maxcount, int* count, int grantedQoSs[], unsigned char* buf, int buflen)
{
	MQTTHeader header = {0};
	unsigned char* curdata = buf;
	unsigned char* enddata = NULL;
	int rc = 0;
	int mylen;

	FUNC_ENTRY;
	header.byte = readChar(&curdata);
	if (header.bits.type != SUBACK)
		goto exit;

	curdata += (rc = MQTTPacket_decodeBuf(curdata, &mylen));
	enddata = curdata + mylen;
	if (enddata - curdata < 2)
		goto exit;

	*packetid = readInt(&curdata);

	*count = 0;
	while (curdata < enddata)
	{
		if (*count > maxcount)
		{
			rc = -1;
			goto exit;
		}
		grantedQoSs[(*count)++] = readChar(&curdata);
	}

	rc = 1;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


