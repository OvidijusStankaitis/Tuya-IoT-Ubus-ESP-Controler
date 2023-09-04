#ifndef _UBUS_TLS_H_
#define _UBUS_TLS_H_

#include <libubus.h>

#define DEVICE_MAX 8
#define ESP_PID 0xEA60
#define ESP_VID 0x10C4

struct device
{
    char port[16];
    char pid[9];
    char vid[9];
};

extern struct blob_buf b;

void init_ubus_methods(struct ubus_context *ctx);

#endif
