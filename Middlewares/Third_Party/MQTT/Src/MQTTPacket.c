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
 *    Sergio R. Caprile - non-blocking packet read functions for stream transport
 *******************************************************************************/

#include "StackTrace.h"
#include "MQTTPacket.h"

#include <string.h>

/**
 * Encodes the message length according to the MQTT algorithm
 * @param buf the buffer into which the encoded data is written
 * @param length the length to be encoded
 * @return the number of bytes written to buffer
 */
int MQTTPacket_encode(unsigned char* buf, int length)
{
	int rc = 0;

	FUNC_ENTRY;
	do
	{
		char d = length % 128;
		length /= 128;
		/* ЕСЛИ ДЛЯ КОДИРОВАНИЯ НУЖНО БОЛЬШЕ ЦИФР УСТАНОВИТЕ ВЕРХНИЙ БИТ БОЛЬШЕ */
		if (length > 0)
			d |= 0x80;
		buf[rc++] = d;
	} while (length > 0);
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
* Декодирует длину сообщения в соответствии с алгоритмом MQTT
 * @param получает указатель charfn на функцию для чтения следующего символа из источника данных
 * @param значение возвращаемой декодированной длины
 * @возвращает количество байт, считанных из сокета
 */
int MQTTPacket_decode(int (*getcharfn)(unsigned char*, int), int* value)
{
	unsigned char c;
	int multiplier = 1;
	int len = 0;
#define MAX_NO_OF_REMAINING_LENGTH_BYTES 4

	FUNC_ENTRY;
	*value = 0;
	do
	{
		int rc = MQTTPACKET_READ_ERROR;

		if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
		{
			rc = MQTTPACKET_READ_ERROR;	/* ОШИБКА ДАННЫХ */
			goto exit;
		}
		rc = (*getcharfn)(&c, 1);
		if (rc != 1)
			goto exit;
		*value += (c & 127) * multiplier;
		multiplier *= 128;
	} while ((c & 128) != 0);
exit:
	FUNC_EXIT_RC(len);
	return len;
}


int MQTTPacket_len(int rem_len)
{
	rem_len += 1;

	if (rem_len < 128)
		rem_len += 1;
	else if (rem_len < 16384)
		rem_len += 2;
	else if (rem_len < 2097151)
		rem_len += 3;
	else
		rem_len += 4;
	return rem_len;
}


static unsigned char* bufptr;

int bufchar(unsigned char* c, int count)
{
	int i;

	for (i = 0; i < count; ++i)
		*c = *bufptr++;
	return count;
}


int MQTTPacket_decodeBuf(unsigned char* buf, int* value)
{
	bufptr = buf;
	return MQTTPacket_decode(bufchar, value);
}


/**
* Вычисляет целое число из двух байтов, считанных из входного буфера
 * @param pptr указатель на входной буфер - увеличивается на количество использованных и возвращенных байтов
 * @возвращает вычисленное целочисленное значение
 */
int readInt(unsigned char** pptr)
{
	unsigned char* ptr = *pptr;
	int len = 256*(*ptr) + (*(ptr+1));
	*pptr += 2;
	return len;
}


/**
* Считывает один символ из входного буфера.
 * @param pptr указатель на входной буфер - увеличивается на количество использованных и возвращенных байтов
 * @возвращает прочитанный символ
 */
char readChar(unsigned char** pptr)
{
	char c = **pptr;
	(*pptr)++;
	return c;
}


/**
* Записывает один символ в выходной буфер.
 * @param pptr указатель на выходной буфер - увеличивается на количество использованных и возвращенных байтов
 * @param c символ для записи
 */
void writeChar(unsigned char** pptr, char c)
{
	**pptr = c;
	(*pptr)++;
}


/**
* Записывает целое число в виде 2 байт в выходной буфер.
 * @param pptr указатель на выходной буфер - увеличивается на количество использованных и возвращенных байтов
 * @param не является целым числом для записи
 */
void writeInt(unsigned char** pptr, int anInt)
{
	**pptr = (unsigned char)(anInt / 256);
	(*pptr)++;
	**pptr = (unsigned char)(anInt % 256);
	(*pptr)++;
}


/**
Записывает строку "UTF" в выходной буфер. Преобразует строку C в строку с разделителями по длине.
 * @param pptr указатель на выходной буфер - увеличивается на количество использованных и возвращенных байтов
 * @param string строка C для записи
 */
void writeCString(unsigned char** pptr, const char* string)
{
	int len = strlen(string);
	writeInt(pptr, len);
	memcpy(*pptr, string, len);
	*pptr += len;
}


int getLenStringLen(char* ptr)
{
	int len = 256*((unsigned char)(*ptr)) + (unsigned char)(*(ptr+1));
	return len;
}


void writeMQTTString(unsigned char** pptr, MQTTString mqttstring)
{
	if (mqttstring.lenstring.len > 0)
	{
		writeInt(pptr, mqttstring.lenstring.len);
		memcpy(*pptr, mqttstring.lenstring.data, mqttstring.lenstring.len);
		*pptr += mqttstring.lenstring.len;
	}
	else if (mqttstring.cstring)
		writeCString(pptr, mqttstring.cstring);
	else
		writeInt(pptr, 0);
}


/**
* @param mqttstring структура MQTTString, в которую должны быть считаны данные
 * @param pptr указатель на выходной буфер - увеличивается на количество использованных и возвращенных байтов
 * @param enddata указатель на конец данных: не считывайте дальше
 * @возвращает 1 в случае успеха, 0 в противном случае
 */
int readMQTTLenString(MQTTString* mqttstring, unsigned char** pptr, unsigned char* enddata)
{
	int rc = 0;

	FUNC_ENTRY;
	/*  первые два байта - это длина строки */
	{
		mqttstring->lenstring.len = readInt(pptr); /* УВЕЛИЧИВАЕТ PPTR ЧТОБЫ УКАЗАТЬ ПРОШЛУЮ ДЛИННУ */
		if (&(*pptr)[mqttstring->lenstring.len] <= enddata)
		{
			mqttstring->lenstring.data = (char*)*pptr;
			*pptr += mqttstring->lenstring.len;
			rc = 1;
		}
	}
	mqttstring->cstring = NULL;
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
* Возвращает длину строки MQTTstring - C, если таковая имеется, в противном случае строка с разделителями длины
 * @param mqttstring строка, возвращающая длину
* @возвращает длину строки
 */
int MQTTstrlen(MQTTString mqttstring)
{
	int rc = 0;

	if (mqttstring.cstring)
		rc = strlen(mqttstring.cstring);
	else
		rc = mqttstring.lenstring.len;
	return rc;
}


/**
* Сравнивает строку MQTTString со строкой C
 * @param - строка MQTT для сравнения
 * @param bptr строка C для сравнения
 * @return boolean - равно или нет
 */
int MQTTPacket_equals(MQTTString* a, char* bptr)
{
	int alen = 0,
		blen = 0;
	char *aptr;
	
	if (a->cstring)
	{
		aptr = a->cstring;
		alen = strlen(a->cstring);
	}
	else
	{
		aptr = a->lenstring.data;
		alen = a->lenstring.len;
	}
	blen = strlen(bptr);
	
	return (alen == blen) && (strncmp(aptr, bptr, alen) == 0);
}


/**
* Вспомогательная функция для считывания пакетных данных из некоторого источника в буфер
* буфер @param buf, в который будет сериализован пакет
* @param buflen длина в байтах предоставленного буфера
* @*@param получить указатель fn на функцию, которая будет считывать любое количество байтов из нужного источника
* @возвращает целочисленный тип пакета MQTT или -1 при ошибке
* @примечание все сообщение должно быть помещено в буфер вызывающего абонента
 */
int MQTTPacket_read(unsigned char* buf, int buflen, int (*getfn)(unsigned char*, int))
{
	int rc = -1;
	MQTTHeader header = {0};
	int len = 0;
	int rem_len = 0;

	/* 1. считайте байт заголовка. В нем указан тип пакета */
	if ((*getfn)(buf, 1) != 1)
		goto exit;

	len = 1;
	/* 2. прочитайте оставшуюся длину.  Это само по себе изменчиво */
	MQTTPacket_decode(getfn, &rem_len);
	len += MQTTPacket_encode(buf + 1, rem_len); /* поместите исходную оставшуюся длину обратно в буфер */

	/* 3. считайте оставшуюся часть буфера, используя обратный вызов для предоставления остальных данных */
	if((rem_len + len) > buflen)
		goto exit;
	if (rem_len && ((*getfn)(buf + len, rem_len) != rem_len))
		goto exit;

	header.byte = buf[0];
	rc = header.bits.type;
exit:
	return rc;
}

/**
* Декодирует длину сообщения в соответствии с алгоритмом MQTT, не блокируя
* @param trp указатель на транспортную структуру, содержащую то, что необходимо для решения проблемы получения из нее данных
* @param значение возвращаемой декодированной длины
* @возвращает целое число - количество байтов, считанных из сокета, 0 при повторном вызове или -1 при ошибке
 */
static int MQTTPacket_decodenb(MQTTTransport *trp)
{
	unsigned char c;
	int rc = MQTTPACKET_READ_ERROR;

	FUNC_ENTRY;
	if(trp->len == 0){		/* инициализировать при первом вызове */
		trp->multiplier = 1;
		trp->rem_len = 0;
	}
	do {
		int frc;
		if (trp->len >= MAX_NO_OF_REMAINING_LENGTH_BYTES)
			goto exit;
		if ((frc=(*trp->getfn)(trp->sck, &c, 1)) == -1)
			goto exit;
		if (frc == 0){
			rc = 0;
			goto exit;
		}
		++(trp->len);
		trp->rem_len += (c & 127) * trp->multiplier;
		trp->multiplier *= 128;
	} while ((c & 128) != 0);
	rc = trp->len;
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}

/**
* Вспомогательная функция для считывания пакетных данных из некоторого источника в буфер, неблокирующая
* @param buf буфер, в который будет сериализован пакет
* @param buflen длина в байтах предоставленного буфера
* @param trp указатель на транспортную структуру, содержащую то, что необходимо для решения проблемы получения из нее данных
* @возвращает целочисленный тип пакета MQTT, 0 для повторного вызова или -1 при ошибке
* @примечание все сообщение должно помещаться в буфер вызывающего абонента
 */
int MQTTPacket_readnb(unsigned char* buf, int buflen, MQTTTransport *trp)
{
	int rc = -1, frc;
	MQTTHeader header = {0};

	switch(trp->state){
	default:
		trp->state = 0;
	case 0:
		/*1 считайте байт заголовка. В нем указан тип пакета */
		if ((frc=(*trp->getfn)(trp->sck, buf, 1)) == -1)
			goto exit;
		if (frc == 0)
			return 0;
		trp->len = 0;
		++trp->state;

		/* 2 считайте оставшуюся длину. Это само по себе изменчиво */
	case 1:
		if((frc=MQTTPacket_decodenb(trp)) == MQTTPACKET_READ_ERROR)
			goto exit;
		if(frc == 0)
			return 0;
		trp->len = 1 + MQTTPacket_encode(buf + 1, trp->rem_len);
		if((trp->rem_len + trp->len) > buflen)
			goto exit;
		++trp->state;

	case 2:
		if(trp->rem_len){
			/* 3 считайте оставшуюся часть буфера, используя обратный вызов для предоставления остальных данных */
			if ((frc=(*trp->getfn)(trp->sck, buf + trp->len, trp->rem_len)) == -1)
				goto exit;
			if (frc == 0)
				return 0;
			trp->rem_len -= frc;
			trp->len += frc;
			if(trp->rem_len)
				return 0;
		}
		header.byte = buf[0];
		rc = header.bits.type;
		break;
	}

exit:
	trp->state = 0;
	return rc;
}

