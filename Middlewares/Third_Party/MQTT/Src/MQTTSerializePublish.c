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
 *    Ian Craggs - fix for https://bugs.eclipse.org/bugs/show_bug.cgi?id=453144
 *******************************************************************************/


#include "StackTrace.h"
#include "MQTTPacket.h"

#include <string.h>


/**
 * Определяет длину пакета публикации MQTT, который будет создан с использованием предоставленных параметров
* @* @param qos QoS MQTT публикации (идентификатор пакета опущен для QoS 0)
* @param topicName имя темы, которое будет использоваться в публикации
* @* @param полезная нагрузка len длина полезной нагрузки, подлежащей отправке
* @возвращает длину буфера, необходимую для хранения сериализованной версии пакета
  */
int MQTTSerialize_publishLength(int qos, MQTTString topicName, int payloadlen)
{
	int len = 0;

	len += 2 + MQTTstrlen(topicName) + payloadlen;
	if (qos > 0)
		len += 2;
	return len;
}


/**
* * Сериализуйте предоставленные данные публикации в предоставленный буфер, готовый к отправке
  * @param buf буфер, в который будет сериализован пакет
  * @*@*@* @param buf len длина в байтах предоставленного буфера
  * @* @param dup целое число - метка dupflag
  * @* @param как целое число - значение QoS MQTT
  * @param сохраненное целое число - флаг сохранения MQTT
  * @param packetid целое число - идентификатор пакета MQTT
  * @* @param topicName MQTTString - встречающаяся тема в публикации
  * байтовый буфер полезной нагрузки @param - полезная нагрузка публикации MQTT
  * @* @param полезная нагрузка len целое число - длина MQTTpayload
  * @возвращает длину сериализованных данных.  <= 0 указывает на ошибку
  */
int MQTTSerialize_publish(unsigned char* buf, int buflen, unsigned char dup, int qos, unsigned char retained, unsigned short packetid,
		MQTTString topicName, unsigned char* payload, int payloadlen)
{
	unsigned char *ptr = buf;
	MQTTHeader header = {0};
	int rem_len = 0;
	int rc = 0;

	FUNC_ENTRY;
	if (MQTTPacket_len(rem_len = MQTTSerialize_publishLength(qos, topicName, payloadlen)) > buflen)
	{
		rc = MQTTPACKET_BUFFER_TOO_SHORT;
		goto exit;
	}

	header.bits.type = PUBLISH;
	header.bits.dup = dup;
	header.bits.qos = qos;
	header.bits.retain = retained;
	writeChar(&ptr, header.byte);

	ptr += MQTTPacket_encode(ptr, rem_len);

	writeMQTTString(&ptr, topicName);

	if (qos > 0)
		writeInt(&ptr, packetid);

	memcpy(ptr, payload, payloadlen);
	ptr += payloadlen;

	rc = ptr - buf;

exit:
	FUNC_EXIT_RC(rc);
	return rc;
}



/**
* Сериализует пакет ack в поставляемый буфер.
* @param buf буфер, в который будет сериализован пакет
* @param buflen длина в байтах предоставленного буфера
* @param введите тип пакета MQTT
* @param dup флаг MQTT dup
* @param packetid идентификатор пакета MQTT
* @возвращает сериализованную длину или ошибку, если 0
*/
int MQTTSerialize_ack(unsigned char* buf, int buflen, unsigned char packettype, unsigned char dup, unsigned short packetid)
{
	MQTTHeader header = {0};
	int rc = 0;
	unsigned char *ptr = buf;

	FUNC_ENTRY;
	if (buflen < 4)
	{
		rc = MQTTPACKET_BUFFER_TOO_SHORT;
		goto exit;
	}
	header.bits.type = packettype;
	header.bits.dup = dup;
	header.bits.qos = (packettype == PUBREL) ? 1 : 0;
	writeChar(&ptr, header.byte);

	ptr += MQTTPacket_encode(ptr, 2);
	writeInt(&ptr, packetid);
	rc = ptr - buf;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
* Сериализует пакет puback в поставляемый буфер.
  * @param buf буфер, в который будет сериализован пакет
  * @param buflen длина в байтах предоставленного буфера
  * @param packetid целое число - идентификатор пакета MQTT
  * @возвращает сериализованную длину или ошибку, если 0
  */
int MQTTSerialize_puback(unsigned char* buf, int buflen, unsigned short packetid)
{
	return MQTTSerialize_ack(buf, buflen, PUBACK, 0, packetid);
}


/**
* Сериализует пакет pubrel в предоставленный буфер.
  * @param buf буфер, в который будет сериализован пакет
  * @param buflen длина в байтах предоставленного буфера
  * @param dup integer - флаг MQTT dup
  * @param packetid целое число - идентификатор пакета MQTT
  * @возвращает сериализованную длину или ошибку, если 0
  */
int MQTTSerialize_pubrel(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid)
{
	return MQTTSerialize_ack(buf, buflen, PUBREL, dup, packetid);
}


/**
* Сериализует пакет pubrel в предоставленный буфер.
  * @param buf буфер, в который будет сериализован пакет
  * @param buflen длина в байтах предоставленного буфера
  * @param packetid целое число - идентификатор пакета MQTT
  * @возвращает сериализованную длину или ошибку, если 0
  */
int MQTTSerialize_pubcomp(unsigned char* buf, int buflen, unsigned short packetid)
{
	return MQTTSerialize_ack(buf, buflen, PUBCOMP, 0, packetid);
}


