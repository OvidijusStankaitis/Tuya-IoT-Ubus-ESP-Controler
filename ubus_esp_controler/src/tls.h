#ifndef _TLS_H_
#define _TLS_H_

#include <libserialport.h>
#include <string.h>

int get_esp_devices(struct sp_port ***port_list);
int send_esp_command(const char *port_name, const char *command);

#endif
