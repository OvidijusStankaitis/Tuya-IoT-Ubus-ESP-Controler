#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <libubus.h>
#include <tuya_error_code.h>
#include <system_interface.h>
#include <mqtt_client_interface.h>
#include <tuyalink_core.h>
#include <tuya_cacert.h>
#include <cJSON.h>


#include "tuyaConnect.h"
#include "ubusInvoke.h"

struct ubus_context *ctx = NULL;
uint32_t id;

static char *deviceId = NULL;

int init_ubus()
{
    ctx = ubus_connect(NULL);
	if (!ctx)
	{
		syslog(LOG_ERR, "Failed to connect to ubus");
		return 1;
	}

	if (ubus_lookup_id(ctx, "esp_device", &id))
	{
		syslog(LOG_ERR, "Failed to lookup 'esp_device' on UBUS.");
		return 1;
	}

    return 0;
}

static char *parse_string(const char *json_string)
{
    cJSON *json_data = cJSON_Parse(json_string);
    if (json_data == NULL)
    {
        syslog(LOG_ERR, "Failed to parse JSON");
        return NULL;
    }

    cJSON *inputParams = cJSON_GetObjectItemCaseSensitive(json_data, "inputParams");
    if (inputParams == NULL)
    {
        syslog(LOG_ERR, "Failed to parse JSON");
        cJSON_Delete(json_data);
        return NULL;
    }

    cJSON *string_item = cJSON_GetObjectItemCaseSensitive(inputParams, "string");
    if (string_item == NULL)
    {
        syslog(LOG_ERR, "Failed to parse JSON");
        cJSON_Delete(json_data);
        return NULL;
    }

    if (!cJSON_IsString(string_item))
    {
        syslog(LOG_ERR, "Failed to parse JSON");
        cJSON_Delete(json_data);
        return NULL;
    }

    char *result = strdup(string_item->valuestring);
    cJSON_Delete(json_data);
    return result;
}

static void send_ubus_command(char *str)
{
    if (!strncmp(str, BASE_METHOD, strlen(BASE_METHOD)))    
    {
        invoke_devices(ctx, id);
        return;
    }
    char *method = strtok(str, DELIMITER);
    char *port = strtok(NULL, DELIMITER);
    char *pin_string = strtok(NULL, DELIMITER);
    int pin = atoi(pin_string);

    invoke_on_off(ctx, id, method, port, pin);
}

static void on_connected(tuya_mqtt_context_t *context, void *user_data)
{
    syslog(LOG_INFO, "Connected");
}

static void on_disconnect(tuya_mqtt_context_t *context, void *user_data)
{
    syslog(LOG_INFO, "Disconnected");
}

static void on_messages(tuya_mqtt_context_t *context, void *user_data, const tuyalink_message_t *msg)
{
    syslog(LOG_INFO, "on message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
    switch (msg->type)
    {
    case THING_TYPE_ACTION_EXECUTE:
    {
        char *result = parse_string(msg->data_string);
        if (result != NULL)
        {
            send_ubus_command(result);
            free(result);
        }
        break;
    }
    default:
        break;
    }
}

int tuya_connect(char *dId, char *dSecret)
{
    int ret = OPRT_OK;
    ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t){
                                     .host = "m1.tuyacn.com",
                                     .port = 8883,
                                     .cacert = tuya_cacert_pem,
                                     .cacert_len = sizeof(tuya_cacert_pem),
                                     .device_id = dId,
                                     .device_secret = dSecret,
                                     .keepalive = 100,
                                     .timeout_ms = 2000,
                                     .on_connected = on_connected,
                                     .on_disconnect = on_disconnect,
                                     .on_messages = on_messages});
    if (ret != OPRT_OK)
    {
        syslog(LOG_ERR, "Failed to initialize Tuya");
        return 1;
    }

    ret = tuya_mqtt_connect(client);
    if (ret != OPRT_OK)
    {
        syslog(LOG_ERR, "Failed to connect to Tuya");
        return 1;
    }

    deviceId = dId;

    return ret;
}

void tuya_log(char str[])
{
    syslog(LOG_INFO, "Sending log");
    char data[256];
    snprintf(data, sizeof(data), "{\"log\":\"%s\"}", str);
    tuyalink_thing_property_report_with_ack(client, deviceId, data);
    syslog(LOG_INFO, "Log sent");
    sleep(1);
}

void ubus_deinit()
{
    ubus_free(ctx);
}