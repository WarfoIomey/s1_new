
#ifndef __MQTTCLIENT_H
#define __MQTTCLIENT_H

#include "main.h"

#include <MQTTPacket.h>
#include "net_conf.h"

#include "usart.h"
#include "esp8266.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MQTT_KeepAliveInterval   60
#define MQTT_MaxWaitCount        0xffff
#define MQTT_PacketBuffSize      1024

#define MQTT_RecvEndFlag         USART3_RecvEndFlag

int mqtt_transport_sendPacketBuffer(uint8_t *buf,int buflen);
int mqtt_transport_getdata(uint8_t *buf, int buflen);
int mqtt_transport_open(uint8_t *host, int port);
int mqtt_transport_close(int sock);

uint8_t mqtt_ConnectServer(void);

#endif
