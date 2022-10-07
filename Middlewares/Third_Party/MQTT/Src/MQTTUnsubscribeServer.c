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
* Десериализует поставляемый (проводной) буфер в данные для отмены подписки.
  * возвращаемое целое число @param dup - флаг MQTT dup
  * возвращаемое целое число @param packetid - идентификатор пакета MQTT
  * @* @param maxcount - максимальное количество участников, разрешенное в фильтрах тем и массивах requestedQoSs.
  * @* @param count - количество участников в фильтрах тем и массивах requestedQoSs
  * @* @param Фильтры тем - массив имен фильтров тем
  * @param buf необработанные данные буфера правильной длины, определяемой полем оставшаяся длина
  * @param buflen длина в байтах данных в предоставленном буфере
  * @возвращает длину сериализованных данных.  <= 0 указывает на ошибку
  */
int MQTTDeserialize_unsubscribe(unsigned char* dup, unsigned short* packetid, int maxcount, int* count, MQTTString topicFilters[],
		unsigned char* buf, int len)
{
	MQTTHeader header = {0};
	unsigned char* curdata = buf;
	unsigned char* enddata = NULL;
	int rc = 0;
	int mylen = 0;

	FUNC_ENTRY;
	header.byte = readChar(&curdata);
	if (header.bits.type != UNSUBSCRIBE)
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
		(*count)++;
	}

	rc = 1;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 * Сериализует предоставленные неподтвержденные данные в поставляемый буфер, готовый к отправке
  * @param buf буфер, в который будет сериализован пакет
  * @param buflen длина в байтах предоставленного буфера
  * @param packetid целое число - идентификатор пакета MQTT
  * @возвращает длину сериализованных данных.  <= 0 указывает на ошибку
  */
int MQTTSerialize_unsuback(unsigned char* buf, int buflen, unsigned short packetid)
{
	MQTTHeader header = {0};
	int rc = 0;
	unsigned char *ptr = buf;

	FUNC_ENTRY;
	if (buflen < 2)
	{
		rc = MQTTPACKET_BUFFER_TOO_SHORT;
		goto exit;
	}
	header.byte = 0;
	header.bits.type = UNSUBACK;
	writeChar(&ptr, header.byte);

	ptr += MQTTPacket_encode(ptr, 2);

	writeInt(&ptr, packetid);

	rc = ptr - buf;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


