#ifndef UBUS_INVOKE_H
#define UBUS_INVOKE_H

#include <libubox/blobmsg_json.h>
#include <libubus.h>

#define BUFFER_SIZE 1024

enum {
    DEVICE_LIST,
    MAX_ESP_DEVICES_ATTR,
};

static const struct blobmsg_policy devices_policy[MAX_ESP_DEVICES_ATTR] = {
    [DEVICE_LIST] = { .name = "devices", .type = BLOBMSG_TYPE_ARRAY },
};

int invoke_devices(struct ubus_context *ctx, uint32_t id);
int invoke_on_off(struct ubus_context *ctx, uint32_t id, char *method, char *argument);

#endif
