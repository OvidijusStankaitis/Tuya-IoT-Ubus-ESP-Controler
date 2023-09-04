#include <cJSON.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>

#include "ubusInvoke.h"

static char devices[BUFFER_SIZE];

static void device_callback(struct ubus_request *req, int type, struct blob_attr *msg)
{
    struct blob_attr *tb[MAX_ESP_DEVICES_ATTR];

    blobmsg_parse(devices_policy, MAX_ESP_DEVICES_ATTR, tb, blob_data(msg), blob_len(msg));

    if (!tb[DEVICE_LIST])
    {
        syslog(LOG_ERR, "Failed to parse devices");
        return;
    }

    char *json_str = blobmsg_format_json(tb[DEVICE_LIST], true);
    snprintf(devices, BUFFER_SIZE, "%s", json_str);
    free(json_str);
}

int invoke_devices(struct ubus_context *ctx, uint32_t id)
{
    if (ubus_invoke(ctx, id, "devices", NULL, device_callback, NULL, 1000))
    {
        syslog(LOG_ERR, "Failed to invoke 'devices' on UBUS.");
        return 1;
    }

    syslog(LOG_INFO, "Devices: %s", devices);
    return 0;
}

int invoke_on_off(struct ubus_context *ctx, uint32_t id, char *method, char *argument)
{
    struct blob_buf b;
    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "argument", argument);

    char *result = NULL;
    if (ubus_invoke(ctx, id, method, b.head, NULL, &result, 1000))
    {
        syslog(LOG_ERR, "Failed to invoke '%s' on UBUS.", method);
        return 1;
    }

    syslog(LOG_INFO, "Result: %s", result);

    free(result);
    return 0;
}