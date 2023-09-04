#include "ubus_tls.h"

struct ubus_context *ctx;

int main(void) {

    const char *ubus_socket = NULL;

    ctx = ubus_connect(ubus_socket);
    
    if (!ctx) {
        fprintf(stderr, "Failed to connect to ubus\n");
        return -1;
    }

    init_ubus_methods(ctx);
    ubus_add_uloop(ctx);
    uloop_run();

    ubus_free(ctx);
    return 0;
}
