#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>

#include "tuyaConnect.h"
#include "argParser.h"

#include "tuyalink_core.h"

#define DATA_LEN 30

tuya_mqtt_context_t client_instance;
tuya_mqtt_context_t *client = &client_instance;
static bool run = true;

static void handle_signal(int sig)
{
	if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT)
	{
		syslog(LOG_INFO, "Received signal to terminate. Disconnecting from Tuya...");
		run = false;
	}
}

int main(int argc, char **argv)
{
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	signal(SIGQUIT, handle_signal);

	openlog("tuya_cloud_daemon", LOG_PID | LOG_CONS, LOG_USER);

	Arguments args;
	parse_arguments(argc, argv, &args);

	syslog(LOG_INFO, "Daemon started");

	int ret = tuya_connect(client, args.deviceId, args.deviceSecret);

	if (ret)
	{
		goto cleanup;
	}

	struct ubus_context *ctx;
	uloop_init();
	uint32_t id;

	ctx = ubus_connect(NULL);
	if (!ctx)
	{
		syslog(LOG_ERR, "Failed to connect to ubus");
		goto cleanup;
	}

	if (ubus_lookup_id(ctx, "esp_device", &id))
	{
		syslog(LOG_ERR, "Failed to lookup 'esp_device' on UBUS.");
		goto cleanup;
	}

	init_ubus(ctx, id);

	while (run)
	{
		tuya_mqtt_loop(client);
		sleep(5);
	}

cleanup:
	tuya_mqtt_disconnect(client);
	tuya_mqtt_deinit(client);
	ubus_free(ctx);
	syslog(LOG_INFO, "Disconnected from Tuya");
	closelog();
	return 0;
}