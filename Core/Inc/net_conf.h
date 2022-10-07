#ifndef __NET_CONF_H
#define __NET_CONF_H

// ИЗМЕНИТЬ  МАКРОС В СООТВЕТСВИИ С ВАШЕМ WIFI

#define WIFI_TAR 1

#if WIFI_TAR == 0
//вай фай найм и пассворд
#define AP_SSID     (const char*)"Honor 8A"
#define AP_PSWD     (const char*)""

#elif WIFI_TAR == 1
//идификация пользователя пк
#define AP_SSID     (const char*)"Honor 8A"
#define AP_PSWD     (const char*)""

#endif

#define CONNECT_MODE 0

#if CONNECT_MODE == 0
//адрес мктт сервера
#define IpServer       "srv2.clusterfly.ru"
#define ServerPort     "9994"

#elif CONNECT_MODE == 1
//адрес пк
#define IpServer       "62.173.145.180"
#define ServerPort     "443"

#endif

//ид девайса

#define MQTT_DEVICE_ID (uint8_t*)"user_f276005d/test"
#define MQTT_SECRET    (uint8_t*)""


//название мктт
#define MQTT_CLIENTID   "clientId-rZ4MfvJoXh"
#define MQTT_USERNAME   "user_f276005d"
#define MQTT_PASSWORD   "pass_10a12988"

#endif
