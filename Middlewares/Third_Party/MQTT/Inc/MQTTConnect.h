/*******************************************************************************
 * Copyright (c) 2014, 2017 IBM Corp.
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
 *    Ian Craggs - add connack return code definitions 
 *    Xiang Rong - 442039 Add makefile to Embedded C client
 *    Ian Craggs - fix for issue #64, bit order in connack response
 *******************************************************************************/
/* ****************
 * ДАННЫЙ БЛОК СЛУЖИТ ДЛЯ ПОДКЛЮЧЕНИЕ ESP К САМОМУ СЕРВЕРУ
 * ***************/
#ifndef MQTTCONNECT_H_
#define MQTTCONNECT_H_
/*************** ПАКЕТ CONNACK ПОДТВЕРЖДЕНИЕ ЗАПРОСА НА ПОДКЛЮЧЕНИЕ***/
enum connack_return_codes
{
    MQTT_CONNECTION_ACCEPTED = 0,      //MQTT ПОДКЛЮЧЕНИЕ ПРИНЯТО
    MQTT_UNNACCEPTABLE_PROTOCOL = 1,   // НЕПРАВИЛЬНЫЙ ПРОТОКОЛ
    MQTT_CLIENTID_REJECTED = 2,        //НЕПРАВЕЛЬНЫЙ КЛИЕНТ ID
    MQTT_SERVER_UNAVAILABLE = 3,       //СЕРВЕР НЕДОСТУПЕН
    MQTT_BAD_USERNAME_OR_PASSWORD = 4, //НЕПРАВИЛЬНЫЙ ЛОГИН ИЛИ ПАРОЛЬ
    MQTT_NOT_AUTHORIZED = 5,           //НЕ АВТОРИЗИРОВАН
};
/*************** ПАКЕТ CONNACK ПОДТВЕРЖДЕНИЕ ЗАПРОСА НА ПОДКЛЮЧЕНИЕ***/

#if !defined(DLLImport)
  #define DLLImport
#endif
#if !defined(DLLExport)
  #define DLLExport
#endif
/***********************************************************
 * Байт Connect Flags содержит ряд параметров, определяющих
 *  поведение MQTT-соединения.
 ***********************************************************/
/****************************ФЛАГИ ПОДКЛЮЧЕНИЯ "CONNECT FLAGS"***********************************************/
typedef union
{
	unsigned char all;
#if defined(REVERSED)
	struct
	{
		unsigned int username : 1;			/**< 3.1 user name */
		unsigned int password : 1; 			/**< 3.1 password */
		unsigned int willRetain : 1;		/**< will retain setting */
		unsigned int willQoS : 2;				/**< will QoS value */
		unsigned int will : 1;			    /**< will flag */
		unsigned int cleansession : 1;	  /**< clean session flag  */
		unsigned int : 1;	  	          /**< unused */
	} bits;
#else
	struct
	{
		unsigned int : 1;	     					/**< unused */
		unsigned int cleansession : 1;	  /**< cleansession flag */
		unsigned int will : 1;			    /**< will flag */
		unsigned int willQoS : 2;				/**< will QoS value */
		unsigned int willRetain : 1;		/**< will retain setting */
		unsigned int password : 1; 			/**< 3.1 password */
		unsigned int username : 1;			/**< 3.1 user name */
	} bits;
#endif

} MQTTConnectFlags;	/**< connect flags byte */
/****************************ФЛАГИ ПОДКЛЮЧЕНИЯ К СЕРВЕРУ***********************************************/


/**
 * ПАКЕТ ПОДЛКЮЧЕНИЯ ИЗДАТЕЛЯ
 */
typedef struct
{

	char struct_id[4];
	/** НОМЕР ВЕРСИИ СТРУКТУРЫ */
	int struct_version;
	/** ТОПИК В КОТОРОМ БУДЕТ ОПУБЛИКОВАННО СООБЩЕНИЕ */
	MQTTString topicName;
	/** СООБЩЕНИЕ КОТОРОЕ БУДЕТ ОПУБЛИКОВАННО В ТОПИКЕ */
	MQTTString message;
	/**
      * СОХРАНЕНИЕ СООБЩЕНИЯ.
      */
	unsigned char retained;
	/**
	 * НАСТРОЙКА QoS ДЛЯ СООБЩЕНИЯ
      */
	char qos;
} MQTTPacket_willOptions;


#define MQTTPacket_willOptions_initializer { {'M', 'Q', 'T', 'W'}, 0, {NULL, {0, NULL}}, {NULL, {0, NULL}}, 0, 0 }

/*
 * ПАКЕТ ПОДКЛЮЧЕНИЯ ПОДПИСЧИКА
 * */
typedef struct
{
	/** The eyecatcher for this structure.  must be MQTC. */
	char struct_id[4];
	/** The version number of this structure.  Must be 0 */
	int struct_version;
	/** Version of MQTT to be used.  3 = 3.1 4 = 3.1.1
	  */
	unsigned char MQTTVersion; 			//ВЕРСИЯ MQTT
	MQTTString clientID; 				// ID КЛИЕНТА
	unsigned short keepAliveInterval;	//ИНТЕРВАЛ ВРЕМЕНИ ДЛЯ ПОДКЛЮЧЕНИЯ
	unsigned char cleansession;			//ОБРАБОТКА СОСТОЯНИЯ СЕАНСА
	unsigned char willFlag;
	/************ФЛАГИ ПОДКЛЮЧЕНИЕ КЛИЕНТА*******************/
	MQTTPacket_willOptions will;
	MQTTString username;
	MQTTString password;
} MQTTPacket_connectData;
/************ФЛАГИ ПОДКЛЮЧЕНИЕ КЛИЕНТА*******************/
/****************************ФЛАГИ ПОДКЛЮЧЕНИЯ "CONNACK"***********************************************/
/*
 * 8-битное беззнаковое значение, представляющее уровень ревизии протокола
 * */
typedef union
{
	unsigned char all;
#if defined(REVERSED)
	struct
	{
    unsigned int reserved : 7;
		unsigned int sessionpresent : 1;
	} bits;
#else
	struct
	{
		unsigned int sessionpresent : 1;
    unsigned int reserved: 7;
	} bits;
#endif
} MQTTConnackFlags;
/****************************ФЛАГИ ПОДКЛЮЧЕНИЯ "CONNACK"***********************************************/

#define MQTTPacket_connectData_initializer { {'M', 'Q', 'T', 'C'}, 0, 4, {NULL, {0, NULL}}, 60, 1, 0, \
		MQTTPacket_willOptions_initializer, {NULL, {0, NULL}}, {NULL, {0, NULL}} }

DLLExport int MQTTSerialize_connect(unsigned char* buf, int buflen, MQTTPacket_connectData* options);
DLLExport int MQTTDeserialize_connect(MQTTPacket_connectData* data, unsigned char* buf, int len);

DLLExport int MQTTSerialize_connack(unsigned char* buf, int buflen, unsigned char connack_rc, unsigned char sessionPresent);
DLLExport int MQTTDeserialize_connack(unsigned char* sessionPresent, unsigned char* connack_rc, unsigned char* buf, int buflen);

DLLExport int MQTTSerialize_disconnect(unsigned char* buf, int buflen);
DLLExport int MQTTSerialize_pingreq(unsigned char* buf, int buflen);

#endif /* MQTTCONNECT_H_ */
