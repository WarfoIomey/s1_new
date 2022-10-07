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
* Десериализует поставляемый (проводной) буфер в данные подписки.
  * возвращаемое целое число @param dup - флаг MQTT dup
  * возвращаемое целое число @param packetid - идентификатор пакета MQTT
  * @* @param maxcount - максимальное количество участников, разрешенное в фильтрах тем и массивах requestedQoSs.
  * @* @param count - количество участников в фильтрах тем и массивах requestedQoSs
  * @* @param Фильтры тем - массив имен фильтров тем
  * @param requestedQoSs - массив запрошенных QoS
  * @param buf необработанные данные буфера правильной длины, определяемой полем оставшаяся длина
  * @param buflen длина в байтах данных в предоставленном буфере
  * @возвращает длину сериализованных данных.  <= 0 указывает на ошибку
  */
int MQTTDeserialize_subscribe(unsigned char* dup, unsigned short* packetid, int maxcount, int* count, MQTTString topicFilters[],
	int requestedQoSs[], unsigned char* buf, int buflen)
{
	MQTTHeader header = {0};
	unsigned char* curdata = buf;
	unsigned char* enddata = NULL;
	int rc = -1;
	int mylen = 0;

	FUNC_ENTRY;
	header.byte = readChar(&curdata);
	if (header.bits.type != SUBSCRIBE)
		goto exit;
	*dup = header.bits.dup;

	curdata += (rc = MQTTPacket_decodeBuf(curdata, &mylen));
	enddata = curdata + mylen;

	*packetid = readInt(&curdata);

	*count = 0;
	while (curdata < enddata)
	{
		if (!readMQTTLenString(&topicFilters[*count], &curdata, enddata))
			goto exit;
		if (curdata >= enddata)
			goto exit;
		requestedQoSs[*count] = readChar(&curdata);
		(*count)++;
	}

	rc = 1;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
* * Сериализует поставляемые данные подпакета в поставляемый буфер, готовый к отправке
  * @param buf буфер, в который будет сериализован пакет
  * @param buflen длина в байтах предоставленного буфера
  * @param packetid целое число - идентификатор пакета MQTT
  * @param count - количество элементов в массиве grantedQoSs
  * @param grantedQoSs - массив предоставленных QoS
  * @возвращает длину сериализованных данных.  <= 0 указывает на ошибку
  */
int MQTTSerialize_suback(unsigned char* buf, int buflen, unsigned short packetid, int count, int* grantedQoSs)
{
	MQTTHeader header = {0};
	int rc = -1;
	unsigned char *ptr = buf;
	int i;

	FUNC_ENTRY;
	if (buflen < 2 + count)
	{
		rc = MQTTPACKET_BUFFER_TOO_SHORT;
		goto exit;
	}
	header.byte = 0;
	header.bits.type = SUBACK;
	writeChar(&ptr, header.byte);

	ptr += MQTTPacket_encode(ptr, 2 + count);

	writeInt(&ptr, packetid);

	for (i = 0; i < count; ++i)
		writeChar(&ptr, grantedQoSs[i]);

	rc = ptr - buf;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


