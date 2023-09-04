#include "ubus_tls.h"
#include "tls.h"
#include <libubox/blobmsg_json.h>
#include <libubox/blobmsg.h>

struct blob_buf b, success, error;

static int ubus_method_devices(struct ubus_context *ctx, struct ubus_object *obj,
                               struct ubus_request_data *req, const char *method,
                               struct blob_attr *msg);
static int ubus_method_on(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg);
static int ubus_method_off(struct ubus_context *ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg);

const struct ubus_method ubus_methods[] = {
    {.name = "devices", .handler = ubus_method_devices},
    {.name = "on", .handler = ubus_method_on},
    {.name = "off", .handler = ubus_method_off},
};

struct ubus_object_type device_object_type =
    UBUS_OBJECT_TYPE("esp_device", ubus_methods);

struct ubus_object device_object = {
    .name = "esp_device",
    .type = &device_object_type,
    .methods = ubus_methods,
    .n_methods = ARRAY_SIZE(ubus_methods),
};

void init_ubus_methods(struct ubus_context *ctx)
{
    ubus_add_object(ctx, &device_object);
    blob_buf_init(&success, 0);
    blobmsg_add_string(&success, "result", "Successfully sent the command");
    blob_buf_init(&error, 0);
    blobmsg_add_string(&error, "error", "Failed to send the command");

}

static int ubus_method_devices(struct ubus_context *ctx, struct ubus_object *obj,
                               struct ubus_request_data *req, const char *method,
                               struct blob_attr *msg)
{
    struct sp_port **ports;
    int i;
    blob_buf_init(&b, 0);
    int count = 0;
    struct device dev[DEVICE_MAX];

    if (get_esp_devices(&ports) < 0)
    {
        blobmsg_add_string(&b, "error", "Failed to get device list");
    }
    else
    {
        void *array = blobmsg_open_array(&b, "devices");

        for (i = 0; ports[i]; i++)
        {
            struct sp_port *port = ports[i];
            int usb_vid, usb_pid;

            char *port_name = sp_get_port_name(port);
            sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);
            if (usb_pid == ESP_PID && usb_vid == ESP_VID && count < DEVICE_MAX)
            {
                char pid[9];
                char vid[9];

                sprintf(pid, "%04X", usb_pid);
                sprintf(vid, "%04X", usb_vid);

                strncpy(dev[count].port, port_name, sizeof(dev[count].port) - 1);
                strncpy(dev[count].pid, pid, sizeof(dev[count].pid) - 1);
                strncpy(dev[count].vid, vid, sizeof(dev[count].vid) - 1);

                void *table = blobmsg_open_table(&b, NULL);

                blobmsg_add_string(&b, "port", dev[count].port);
                blobmsg_add_string(&b, "product id", dev[count].pid);
                blobmsg_add_string(&b, "vendor id", dev[count].vid);

                blobmsg_close_table(&b, table);
                count++;
            }
        }

        blobmsg_close_array(&b, array);
        sp_free_port_list(ports);
    }

    ubus_send_reply(ctx, req, b.head);
    return 0;
}

static int send_command_to_device(const char *port_name, const char *action, int pin)
{
    char command[64];
    snprintf(command, sizeof(command), "{\"action\": \"%s\", \"pin\": %d}", action, pin);
    return send_esp_command(port_name, command);
}

const struct blobmsg_policy ubus_method_policy[] = {
    {.name = "port", .type = BLOBMSG_TYPE_STRING},
    {.name = "pin", .type = BLOBMSG_TYPE_INT32},
};

static int ubus_method_on(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
    struct blob_attr *tb[2];
    const char *port_name;
    int pin;

    blobmsg_parse(ubus_method_policy, ARRAY_SIZE(ubus_method_policy), tb, blob_data(msg), blob_len(msg));
    if (!tb[0] || !tb[1])
    {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    port_name = blobmsg_get_string(tb[0]);
    pin = blobmsg_get_u32(tb[1]);

    if (pin < 0 || pin > 16 ||
        pin != 0 && pin != 2 &&
            pin != 4 && pin != 5 &&
            pin != 12 && pin != 13 &&
            pin != 14 && pin != 15 &&
            pin != 16)
    {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    if (send_command_to_device(port_name, "on", pin) < 0)
    {
        ubus_send_reply(ctx, req, error.head);
    }
    else
    {
        ubus_send_reply(ctx, req, success.head);
    }

    return 0;
}

static int ubus_method_off(struct ubus_context *ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg)
{
    struct blob_attr *tb[2];
    const char *port_name;
    int pin;

    blobmsg_parse(ubus_method_policy, ARRAY_SIZE(ubus_method_policy), tb, blob_data(msg), blob_len(msg));
    if (!tb[0] || !tb[1])
    {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    port_name = blobmsg_get_string(tb[0]);
    pin = blobmsg_get_u32(tb[1]);

    if (pin < 0 || pin > 16 ||
        pin != 0 && pin != 2 &&
            pin != 4 && pin != 5 &&
            pin != 12 && pin != 13 &&
            pin != 14 && pin != 15 &&
            pin != 16)
    {
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    if (send_command_to_device(port_name, "off", pin) < 0)
    {
        ubus_send_reply(ctx, req, error.head);
    }
    else
    {
        ubus_send_reply(ctx, req, success.head);
    }

    return 0;
}