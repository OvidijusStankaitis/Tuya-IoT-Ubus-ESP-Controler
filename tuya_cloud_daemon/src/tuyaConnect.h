#include "tuyalink_core.h"

#ifndef TUYA_CONNECT_H
#define TUYA_CONNECT_H

#define DATA_LEN 30
#define DELIMITER " "
#define BASE_METHOD "devices"

struct Device
{
  char productId[DATA_LEN];
  char deviceId[DATA_LEN];
  char deviceSecret[DATA_LEN];
};

extern tuya_mqtt_context_t *client;

int init_ubus();
int tuya_connect(char *dId, char *dSecret);
void tuya_log(char str[]);
void ubus_deinit();

#endif
