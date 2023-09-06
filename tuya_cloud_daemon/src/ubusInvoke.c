#include <cJSON.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>

#include "ubusInvoke.h"
#include "tuyaConnect.h"

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

static int parse_devices()
{
    cJSON *root = cJSON_Parse(devices);
    if (!root)
    {
        syslog(LOG_ERR, "Failed to parse JSON");
        return 1;
    }

    if (!cJSON_IsArray(root))
    {
        cJSON_Delete(root);
        syslog(LOG_ERR, "JSON is not an array");
        return 1;
    }

    int deviceCount = cJSON_GetArraySize(root);
    for (int i = 0; i < deviceCount; i++)
    {
        cJSON *device = cJSON_GetArrayItem(root, i);
        cJSON *port = cJSON_GetObjectItemCaseSensitive(device, "port");
        cJSON *productId = cJSON_GetObjectItemCaseSensitive(device, "product id");
        cJSON *vendorId = cJSON_GetObjectItemCaseSensitive(device, "vendor id");

        if (cJSON_IsString(port) && cJSON_IsString(productId) && cJSON_IsString(vendorId))
        {
            char csv[BUFFER_SIZE];
            int csvIndex = snprintf(csv, BUFFER_SIZE, "Device: %d, port: %s, product id: %s, vendor id: %s",
                                     i + 1, port->valuestring, productId->valuestring, vendorId->valuestring);

            syslog(LOG_INFO, "%s", csv);
            tuya_log(csv);
            memset(csv, 0, BUFFER_SIZE);
        }
    }

    memset(devices, 0, BUFFER_SIZE);
    cJSON_Delete(root);
    return 0;
}

int invoke_devices(struct ubus_context *ctx, uint32_t id)
{
    if (ubus_invoke(ctx, id, "devices", NULL, device_callback, &devices, 1000))
    {
        syslog(LOG_ERR, "Failed to invoke 'devices' on UBUS.");
        tuya_log("Failed to invoke 'devices' on UBUS.");
        return 1;
    }

    if(parse_devices())
    {
        syslog(LOG_ERR, "Failed to parse devices");
        tuya_log("Failed to parse devices");
        return 1;
    }
    

    syslog(LOG_INFO, "Successfully invoked 'devices' on UBUS.");
    tuya_log("Successfully invoked 'devices' on UBUS.");
    return 0;
}

int invoke_on_off(struct ubus_context *ctx, uint32_t id, char *method, char *port, int *pin)
{
    struct blob_buf b = {};
    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "port", port);
    blobmsg_add_u32(&b, "pin", pin);

    if (ubus_invoke(ctx, id, method, b.head, NULL, NULL, 3000))
    {
        if(method == "on")
        {
            syslog(LOG_ERR, "Failed to invoke 'on' on UBUS.");
            tuya_log("Failed to invoke 'on' on UBUS.");
            return 1;
        }
        else
        {
            syslog(LOG_ERR, "Failed to invoke 'off' on UBUS.");
            tuya_log("Failed to invoke 'off' on UBUS.");
            return 1;
        }
    }

    if(method == "on")
    {
        syslog(LOG_INFO, "Successfully invoked 'on' on UBUS.");
        tuya_log("Successfully invoked 'on' on UBUS.");
    }
    else
    {
        syslog(LOG_INFO, "Successfully invoked 'off' on UBUS.");
        tuya_log("Successfully invoked 'off' on UBUS.");
    }

    
    return 0;
}