#include <argp.h>
#include <syslog.h>
#include <stdlib.h>

#include "argParser.h"

static struct argp_option options[] = {
    {"deviceId", 'd', "DEVICEID", 0, "Device ID"},
    {"productId", 'p', "PRODUCTID", 0, "Product ID"},
    {"deviceScrt", 's', "DEVICESCRT", 0, "Device Secret"},
    {0}};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    Arguments *args = state->input;
    switch (key)
    {
    case 'd':
        args->deviceId = arg;
        break;
    case 'p':
        args->productId = arg;
        break;
    case 's':
        args->deviceSecret = arg;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, 0};

void parse_arguments(int argc, char **argv, Arguments *args)
{
    argp_parse(&argp, argc, argv, 0, 0, args);
    if (!args->deviceId || !args->productId || !args->deviceSecret)
    {
            syslog(LOG_ERR, "Unspecified arguments");
            printf("tuya_cloud_daemon: too few arguments specified\n");
            printf("tuya_cloud_daemon: Use -? or --help or --usage for more information.\n");
            closelog();
            exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "Arguments provided");
}
