#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "sa_partie.h"

int main(int argc, char **argv) {
    const char *ip;
    short port;
    int expected_players;
    sa_ctx_t ctx;

    signal(SIGPIPE, SIG_IGN);

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <nb_joueurs_attendus>\n", argv[0]);
        return 1;
    }

    ip = argv[1];
    port = (short)atoi(argv[2]);
    expected_players = atoi(argv[3]);

    if (sa_init(&ctx, ip, port, expected_players) != 0) {
        return 1;
    }

    sa_lobby(&ctx);
    sa_run_game(&ctx);
    sa_shutdown(&ctx);
    return 0;
}
