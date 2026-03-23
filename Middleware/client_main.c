#include "middleware_client.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int port;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip_hote> <port_hote>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[2]);
    return client_run(argv[1], port);
}
