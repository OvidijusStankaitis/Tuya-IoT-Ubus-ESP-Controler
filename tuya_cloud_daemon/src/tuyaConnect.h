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

void init_ubus(struct ubus_context *ctxS, uint32_t idS);
int tuya_connect(char *dId, char *dSecret);
void tuya_log(char str[]);

#endif
