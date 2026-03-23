#include "sa_partie.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

static int sa_count_connected(const sa_ctx_t *ctx) {
    int i;
    int count = 0;

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        if (ctx->clients[i].used) {
            count++;
        }
    }

    return count;
}

static int sa_find_free_slot(const sa_ctx_t *ctx) {
    int i;

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        if (!ctx->clients[i].used) {
            return i;
        }
    }

    return -1;
}

static void sa_build_next_player_id(const sa_ctx_t *ctx, char *player_id, size_t player_id_size) {
    int number = 1;

    while (number <= MW_MAX_PLAYERS) {
        int used = 0;
        int i;
        char candidate[MW_MAX_PLAYER_ID_LEN];

        snprintf(candidate, sizeof(candidate), "J%d", number);
        for (i = 0; i < MW_MAX_PLAYERS; i++) {
            if (ctx->clients[i].used && strcmp(ctx->clients[i].player_id, candidate) == 0) {
                used = 1;
                break;
            }
        }

        if (!used) {
            proto_copy_text(player_id, player_id_size, candidate);
            return;
        }

        number++;
    }

    player_id[0] = '\0';
}

static int sa_create_listen_socket(const char *ip, short port) {
    int listen_fd;
    int reuse = 1;
    struct sockaddr_in address;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons((unsigned short)port);

    if (ip == NULL || ip[0] == '\0' || strcmp(ip, "0.0.0.0") == 0) {
        address.sin_addr.s_addr = htonl(INADDR_ANY);
    } else if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0) {
        fprintf(stderr, "[SA] Adresse IP invalide: %s\n", ip);
        close(listen_fd);
        return -1;
    }

    if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) != 0) {
        perror("bind");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, MW_MAX_PLAYERS) != 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    return listen_fd;
}

static void sa_close_client(client_conn_t *client) {
    if (client->used && client->socket_fd >= 0) {
        close(client->socket_fd);
    }

    memset(client, 0, sizeof(*client));
    client->socket_fd = -1;
}

static void sa_remove_lobby_client(sa_ctx_t *ctx, int index) {
    if (!ctx->clients[index].used) {
        return;
    }

    printf("[SA] %s quitte le lobby.\n", ctx->clients[index].player_id);
    game_remove_player(ctx->game, ctx->clients[index].player_id);
    sa_close_client(&ctx->clients[index]);
}

static void sa_remove_game_client(sa_ctx_t *ctx, int index) {
    if (!ctx->clients[index].used) {
        return;
    }

    printf("[SA] %s deconnecte pendant la partie.\n", ctx->clients[index].player_id);
    game_mark_disconnected(ctx->game, ctx->clients[index].player_id);
    sa_close_client(&ctx->clients[index]);
}

static int sa_send_message_to_slot(sa_ctx_t *ctx, int slot, const char *message, int in_game) {
    if (!ctx->clients[slot].used) {
        return -1;
    }

    if (proto_send_cstr(ctx->clients[slot].socket_fd, message) != 0) {
        if (in_game) {
            sa_remove_game_client(ctx, slot);
        } else {
            sa_remove_lobby_client(ctx, slot);
        }
        return -1;
    }

    return 0;
}

static void sa_accept_one(sa_ctx_t *ctx, int lobby_open) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd;
    char buffer[MW_MAX_MSG_LEN];
    char response[MW_MAX_MSG_LEN];
    char player_id[MW_MAX_PLAYER_ID_LEN];
    int ready = 0;
    int slot;

    client_fd = accept(ctx->listen_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        return;
    }

    if (proto_recv_cstr(client_fd, buffer, sizeof(buffer)) != 1 || proto_parse_join(buffer, &ready) != 0 || !ready) {
        proto_build_join_ko(response, sizeof(response), "bad_req");
        proto_send_cstr(client_fd, response);
        close(client_fd);
        return;
    }

    if (!lobby_open) {
        proto_build_join_ko(response, sizeof(response), "partie_lancee");
        proto_send_cstr(client_fd, response);
        close(client_fd);
        return;
    }

    if (sa_count_connected(ctx) >= ctx->expected_players) {
        proto_build_join_ko(response, sizeof(response), "partie_pleine");
        proto_send_cstr(client_fd, response);
        close(client_fd);
        return;
    }

    slot = sa_find_free_slot(ctx);
    if (slot < 0) {
        proto_build_join_ko(response, sizeof(response), "partie_pleine");
        proto_send_cstr(client_fd, response);
        close(client_fd);
        return;
    }

    sa_build_next_player_id(ctx, player_id, sizeof(player_id));
    if (player_id[0] == '\0' || game_add_player(ctx->game, player_id) != 0) {
        proto_build_join_ko(response, sizeof(response), "game_error");
        proto_send_cstr(client_fd, response);
        close(client_fd);
        return;
    }

    ctx->clients[slot].used = 1;
    ctx->clients[slot].socket_fd = client_fd;
    proto_copy_text(ctx->clients[slot].player_id, sizeof(ctx->clients[slot].player_id), player_id);

    proto_build_join_ok(response, sizeof(response), player_id);
    proto_send_cstr(client_fd, response);
    printf("[SA] %s connecte depuis %s.\n", player_id, inet_ntoa(client_addr.sin_addr));
}

static void sa_handle_lobby_clients(sa_ctx_t *ctx, fd_set *read_set) {
    int i;

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        char buffer[MW_MAX_MSG_LEN];
        int status;

        if (!ctx->clients[i].used || !FD_ISSET(ctx->clients[i].socket_fd, read_set)) {
            continue;
        }

        status = proto_recv_cstr(ctx->clients[i].socket_fd, buffer, sizeof(buffer));
        if (status <= 0) {
            sa_remove_lobby_client(ctx, i);
        }
    }
}

static void sa_build_fdset(sa_ctx_t *ctx, fd_set *read_set, int *max_fd, int include_stdin) {
    int i;

    FD_ZERO(read_set);
    FD_SET(ctx->listen_fd, read_set);
    *max_fd = ctx->listen_fd;

    if (include_stdin) {
        FD_SET(STDIN_FILENO, read_set);
        if (STDIN_FILENO > *max_fd) {
            *max_fd = STDIN_FILENO;
        }
    }

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        if (!ctx->clients[i].used) {
            continue;
        }

        FD_SET(ctx->clients[i].socket_fd, read_set);
        if (ctx->clients[i].socket_fd > *max_fd) {
            *max_fd = ctx->clients[i].socket_fd;
        }
    }
}

static int sa_client_is_alive(const sa_ctx_t *ctx, int slot) {
    const PlayerState *player;

    if (!ctx->clients[slot].used) {
        return 0;
    }

    player = game_find_player_const(ctx->game, ctx->clients[slot].player_id);
    return player != NULL && player->alive;
}

static void sa_record_round_participants(const sa_ctx_t *ctx, int participants[MW_MAX_PLAYERS]) {
    int i;

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        participants[i] = sa_client_is_alive(ctx, i);
    }
}

static void sa_send_question(sa_ctx_t *ctx, const int participants[MW_MAX_PLAYERS], const char *message) {
    int i;

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        if (participants[i]) {
            sa_send_message_to_slot(ctx, i, message, 1);
        }
    }
}

static void sa_send_round_results(sa_ctx_t *ctx, const int participants[MW_MAX_PLAYERS]) {
    int i;

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        char buffer[MW_MAX_MSG_LEN];

        if (!participants[i] || !ctx->clients[i].used) {
            continue;
        }

        if (game_prepare_result_msg(ctx->game, ctx->clients[i].player_id, buffer, sizeof(buffer)) < 0) {
            continue;
        }

        sa_send_message_to_slot(ctx, i, buffer, 1);
    }
}

static void sa_send_end_messages(sa_ctx_t *ctx) {
    int i;

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        char buffer[MW_MAX_MSG_LEN];

        if (!ctx->clients[i].used) {
            continue;
        }

        if (game_prepare_end_msg(ctx->game, ctx->clients[i].player_id, buffer, sizeof(buffer)) < 0) {
            continue;
        }

        sa_send_message_to_slot(ctx, i, buffer, 1);
    }
}

static void sa_handle_round_activity(sa_ctx_t *ctx, fd_set *read_set) {
    int i;

    if (FD_ISSET(ctx->listen_fd, read_set)) {
        sa_accept_one(ctx, 0);
    }

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        char buffer[MW_MAX_MSG_LEN];
        int round_id = 0;
        char choice = '\0';
        int status;

        if (!ctx->clients[i].used || !FD_ISSET(ctx->clients[i].socket_fd, read_set)) {
            continue;
        }

        status = proto_recv_cstr(ctx->clients[i].socket_fd, buffer, sizeof(buffer));
        if (status <= 0) {
            sa_remove_game_client(ctx, i);
            continue;
        }

        if (proto_parse_answer(buffer, &round_id, &choice) == 0 &&
            round_id == ctx->game->current_question_index + 1) {
            choice = (char)toupper((unsigned char)choice);
            game_process_answer(ctx->game, ctx->clients[i].player_id, choice);
        }
    }
}

int sa_init(sa_ctx_t *ctx, const char *ip, short port, int expected_players) {
    int i;

    if (ctx == NULL || expected_players <= 0 || expected_players > MW_MAX_PLAYERS) {
        return -1;
    }

    memset(ctx, 0, sizeof(*ctx));
    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        ctx->clients[i].socket_fd = -1;
    }

    ctx->expected_players = expected_players;
    ctx->listen_fd = sa_create_listen_socket(ip, port);
    if (ctx->listen_fd < 0) {
        return -1;
    }

    ctx->game = game_init();
    if (ctx->game == NULL) {
        close(ctx->listen_fd);
        ctx->listen_fd = -1;
        return -1;
    }

    printf("[SA] En attente de %d joueur(s) sur %s:%hd\n", expected_players, ip, port);
    return 0;
}

void sa_lobby(sa_ctx_t *ctx) {
    printf("[SA] Lobby actif. ENTER pour lancer quand tous les joueurs sont connectes.\n");

    while (1) {
        fd_set read_set;
        int max_fd;
        int ready;

        sa_build_fdset(ctx, &read_set, &max_fd, 1);
        ready = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select");
            return;
        }

        if (FD_ISSET(ctx->listen_fd, &read_set)) {
            sa_accept_one(ctx, 1);
        }

        sa_handle_lobby_clients(ctx, &read_set);

        if (FD_ISSET(STDIN_FILENO, &read_set)) {
            char line[64];
            (void)fgets(line, sizeof(line), stdin);

            if (sa_count_connected(ctx) >= ctx->expected_players) {
                if (game_start(ctx->game) == 0) {
                    ctx->started = 1;
                }
                return;
            }

            printf("[SA] Pas assez de joueurs (%d/%d).\n", sa_count_connected(ctx), ctx->expected_players);
        }
    }
}

void sa_run_game(sa_ctx_t *ctx) {
    if (ctx == NULL || !ctx->started) {
        return;
    }

    while (!game_is_finished(ctx->game)) {
        char question_msg[MW_MAX_MSG_LEN];
        int participants[MW_MAX_PLAYERS];
        long deadline_ms;

        if (game_prepare_question_msg(ctx->game, question_msg, sizeof(question_msg)) < 0) {
            break;
        }

        sa_record_round_participants(ctx, participants);
        sa_send_question(ctx, participants, question_msg);

        deadline_ms = proto_now_ms() + (long)MW_ROUND_DURATION_SECONDS * 1000L;

        while (!game_all_alive_answered(ctx->game)) {
            fd_set read_set;
            int max_fd;
            struct timeval timeout;
            long remaining_ms = deadline_ms - proto_now_ms();
            int ready;

            if (remaining_ms <= 0) {
                break;
            }

            timeout.tv_sec = (time_t)(remaining_ms / 1000L);
            timeout.tv_usec = (suseconds_t)((remaining_ms % 1000L) * 1000L);

            sa_build_fdset(ctx, &read_set, &max_fd, 0);
            ready = select(max_fd + 1, &read_set, NULL, NULL, &timeout);
            if (ready < 0) {
                if (errno == EINTR) {
                    continue;
                }
                perror("select");
                break;
            }

            if (ready == 0) {
                break;
            }

            sa_handle_round_activity(ctx, &read_set);
        }

        game_evaluate_round(ctx->game);
        sa_send_round_results(ctx, participants);

        if (game_is_finished(ctx->game) || game_next_question(ctx->game) != 0) {
            break;
        }
    }

    sa_send_end_messages(ctx);
    printf("[SA] Partie terminee.\n");
}

void sa_shutdown(sa_ctx_t *ctx) {
    int i;

    if (ctx == NULL) {
        return;
    }

    for (i = 0; i < MW_MAX_PLAYERS; i++) {
        sa_close_client(&ctx->clients[i]);
    }

    if (ctx->listen_fd >= 0) {
        close(ctx->listen_fd);
        ctx->listen_fd = -1;
    }

    if (ctx->game != NULL) {
        game_cleanup(ctx->game);
        ctx->game = NULL;
    }
}
