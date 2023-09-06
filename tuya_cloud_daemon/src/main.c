#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include <tuyalink_core.h>

#include "tuyaConnect.h"
#include "argParser.h"


#define DATA_LEN 30

tuya_mqtt_context_t client_instance;
tuya_mqtt_context_t *client = &client_instance;

extern tuya_mqtt_context_t *client;

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

	int ret = tuya_connect(args.deviceId, args.deviceSecret);

	if (ret)
	{
		goto cleanup;
	}

	uloop_init();

	if(init_ubus())
	{
		goto cleanup;
	}

	while (run)
	{
		tuya_mqtt_loop(client);
		sleep(1);
	}

cleanup:
	tuya_mqtt_disconnect(client);
	tuya_mqtt_deinit(client);
	ubus_deinit();
	syslog(LOG_INFO, "Disconnected from Tuya");
	closelog();
	return 0;
}