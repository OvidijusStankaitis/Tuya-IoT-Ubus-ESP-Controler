#include "tls.h"

int get_esp_devices(struct sp_port ***port_list)
{
    return sp_list_ports(port_list);
}

int send_esp_command(const char *port_name, const char *command)
{
    struct sp_port *port;
    if (sp_get_port_by_name(port_name, &port) != SP_OK)
    {
        return -1;
    }
    if (sp_open(port, SP_MODE_WRITE) != SP_OK)
    {
        return -2;
    }
    sp_nonblocking_write(port, command, strlen(command));
    sp_drain(port);
    sp_close(port);
    return 0;
}
