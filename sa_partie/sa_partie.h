#ifndef SA_PARTIE_H
#define SA_PARTIE_H

#include "../Middleware/middleware_protocol.h"
#include "../game_engine/game_engine.h"

typedef struct {
    int used;
    int socket_fd;
    char player_id[MW_MAX_PLAYER_ID_LEN];
} client_conn_t;

typedef struct {
    int listen_fd;
    int expected_players;
    int started;
    client_conn_t clients[MW_MAX_PLAYERS];
    GameState *game;
} sa_ctx_t;

int sa_init(sa_ctx_t *ctx, const char *ip, short port, int expected_players);
void sa_lobby(sa_ctx_t *ctx);
void sa_run_game(sa_ctx_t *ctx);
void sa_shutdown(sa_ctx_t *ctx);

#endif
