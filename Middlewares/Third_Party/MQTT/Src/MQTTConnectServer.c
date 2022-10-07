/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "StackTrace.h"
#include "MQTTPacket.h"
#include <string.h>

#define min(a, b) ((a < b) ? a : b)


/**
  * Validates MQTT protocol name and version combinations
  * @param protocol the MQTT protocol name as an MQTTString
  * @param version the MQTT protocol version number, as in the connect packet
  * @return correct MQTT combination?  1 is true, 0 is false
  */
/************************ПРОВЕРКА ВЕРСИИ КЛИЕНТА*******************************************/
int MQTTPacket_checkVersion(MQTTString* protocol, int version)
{
	int rc = 0;

	if (version == 3 && memcmp(protocol->lenstring.data, "MQIsdp",
			min(6, protocol->lenstring.len)) == 0)
		rc = 1;
	else if (version == 4 && memcmp(protocol->lenstring.data, "MQTT",
			min(4, protocol->lenstring.len)) == 0)
		rc = 1;
	return rc;
}
/************************ПРОВЕРКА ВЕРСИИ КЛИЕНТА*******************************************/

/**
  * Deserializes the supplied (wire) buffer into connect data structure
  * @param data the connect data structure to be filled out
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param len the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
/*********************************ПОДКЛЮЧЕНИЕ К СЕРВЕРУ*************************************************/
int MQTTDeserialize_connect(MQTTPacket_connectData* data, unsigned char* buf, int len)
{
	MQTTHeader header = {0};
	MQTTConnectFlags flags = {0};
	unsigned char* curdata = buf;
	unsigned char* enddata = &buf[len];
	int rc = 0;
	MQTTString Protocol;
	int version;
	int mylen = 0;

	FUNC_ENTRY;
	header.byte = readChar(&curdata);
	if (header.bits.type != CONNECT)
		goto exit;

	curdata += MQTTPacket_decodeBuf(curdata, &mylen);

	if (!readMQTTLenString(&Protocol, &curdata, enddata) ||
		enddata - curdata < 0)
		goto exit;

	version = (int)readChar(&curdata); /* ВЕРСИЯ ПРОТОКОЛА */
	/* ЕСЛИ МЫ НЕ РАСПОЗНАЕМ ВЕРСИЮ ПРОТКОЛА, МЫ НЕ АНАЛИЗИРУЕМ ПАКЕТ ПОДКЛЮЧЕНИЯ
	 *  НА ОСНОВАНИИ ТОГО, ЧТО МЫ ЗНАЕМ КАКИМ БУДЕТ ФОРМАТ
	 */
	if (MQTTPacket_checkVersion(&Protocol, version))
	{
		flags.all = readChar(&curdata);
		data->cleansession = flags.bits.cleansession;
		data->keepAliveInterval = readInt(&curdata);
		if (!readMQTTLenString(&data->clientID, &curdata, enddata))
			goto exit;
		data->willFlag = flags.bits.will;
		if (flags.bits.will)
		{
			data->will.qos = flags.bits.willQoS;
			data->will.retained = flags.bits.willRetain;
			if (!readMQTTLenString(&data->will.topicName, &curdata, enddata) ||
				  !readMQTTLenString(&data->will.message, &curdata, enddata))
				goto exit;
		}
		if (flags.bits.username)
		{
			if (enddata - curdata < 3 || !readMQTTLenString(&data->username, &curdata, enddata))
				goto exit; /* ФЛАГ ИМЕНИ ПОЛЬЗОВАТЕЛЯ УСТАНОВЛЕН, НО ИМЯ ПОЛЬЗОВАТЕЛЯ НЕ УКАЗАН - НЕДОПУСТИМО*/
			if (flags.bits.password &&
				(enddata - curdata < 3 || !readMQTTLenString(&data->password, &curdata, enddata)))
				goto exit; /* ФЛАГ ПАРОЛЯ УСТАНОВЛЕ, НО ПАРОЛЬ НЕ УКАЗАН - НЕДОПУСТИМО */
		}
		else if (flags.bits.password)
			goto exit; /* ФЛАГ ПАРОЛЯ УСТАНОВЛЕН БЕЗ ИМЕНИ ПОЛЬЗОВАТЕЛЯ - НЕДОПУСТИМО */
		rc = 1;
	}
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
  Сериализует пакет connack в поставляемый буфер.
 * @param buf буфер, в который будет сериализован пакет
 * @param buflen длина в байтах предоставленного буфера
 * @param connack_rc целочисленный код возврата connack, который будет использоваться
 * @param sessionпредставляет флаг MQTT 3.1.1 sessionPresent
 * @возвращает сериализованную длину или ошибку, если 0
  */
int MQTTSerialize_connack(unsigned char* buf, int buflen, unsigned char connack_rc, unsigned char sessionPresent)
{
	MQTTHeader header = {0};
	int rc = 0;
	unsigned char *ptr = buf;
	MQTTConnackFlags flags = {0};

	FUNC_ENTRY;
	if (buflen < 2)
	{
		rc = MQTTPACKET_BUFFER_TOO_SHORT;
		goto exit;
	}
	header.byte = 0;
	header.bits.type = CONNACK;
	writeChar(&ptr, header.byte); /* write header */

	ptr += MQTTPacket_encode(ptr, 2); /* write remaining length */

	flags.all = 0;
	flags.bits.sessionpresent = sessionPresent;
	writeChar(&ptr, flags.all); 
	writeChar(&ptr, connack_rc);

	rc = ptr - buf;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}

