#include "middleware_client.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../Test_complet/game_runtime.h"
#include "middleware_protocol.h"

typedef struct {
    int socket_fd;
    char player_id[MW_MAX_PLAYER_ID_LEN];
    int alive;
    int score;
    int hardware_ready;
    int has_current_question;
    DisplayedQuestion current_question;
    GameRoundAnswer last_answer;
} ClientSession;

static int client_connect_socket(const char *host_ip, int host_port) {
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *cursor;
    char port_text[16];
    int socket_fd = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    snprintf(port_text, sizeof(port_text), "%d", host_port);
    if (getaddrinfo(host_ip, port_text, &hints, &result) != 0) {
        return -1;
    }

    for (cursor = result; cursor != NULL; cursor = cursor->ai_next) {
        socket_fd = socket(cursor->ai_family, cursor->ai_socktype, cursor->ai_protocol);
        if (socket_fd < 0) {
            continue;
        }

        if (connect(socket_fd, cursor->ai_addr, cursor->ai_addrlen) == 0) {
            break;
        }

        close(socket_fd);
        socket_fd = -1;
    }

    freeaddrinfo(result);
    return socket_fd;
}

static void client_fill_question(const ProtoQuestion *proto_question, DisplayedQuestion *question) {
    char *answer_ptrs[GAME_MAX_ANSWERS];
    int i;

    for (i = 0; i < GAME_MAX_ANSWERS; i++) {
        answer_ptrs[i] = (char *)proto_question->answers[i];
    }

    game_build_question(question,
                        proto_question->difficulty,
                        proto_question->question,
                        answer_ptrs,
                        proto_question->answer_count,
                        '\0');
}

int client_run(const char *host_ip, int host_port) {
    ClientSession session;
    char buffer[MW_MAX_MSG_LEN];
    char cause[64];

    memset(&session, 0, sizeof(session));

    session.socket_fd = client_connect_socket(host_ip, host_port);
    if (session.socket_fd < 0) {
        fprintf(stderr, "[CLIENT] Impossible de se connecter a %s:%d\n", host_ip, host_port);
        return 1;
    }

    proto_build_join(buffer, sizeof(buffer));
    if (proto_send_cstr(session.socket_fd, buffer) != 0) {
        close(session.socket_fd);
        return 1;
    }

    if (proto_recv_cstr(session.socket_fd, buffer, sizeof(buffer)) != 1) {
        close(session.socket_fd);
        return 1;
    }

    if (proto_parse_join_ok(buffer, session.player_id, sizeof(session.player_id)) != 0) {
        if (proto_parse_join_ko(buffer, cause, sizeof(cause)) == 0) {
            fprintf(stderr, "[CLIENT] JOIN refuse: %s\n", cause);
        } else {
            fprintf(stderr, "[CLIENT] Reponse JOIN invalide.\n");
        }
        close(session.socket_fd);
        return 1;
    }

    session.alive = 1;
    printf("[CLIENT] Connecte comme %s\n", session.player_id);

    game_init_hardware();
    session.hardware_ready = 1;

    while (1) {
        int receive_status = proto_recv_cstr(session.socket_fd, buffer, sizeof(buffer));
        int code;

        if (receive_status != 1) {
            fprintf(stderr, "[CLIENT] Connexion perdue avec l'hote.\n");
            break;
        }

        code = proto_get_code(buffer);
        if (code == PROTO_MSG_QUESTION) {
            ProtoQuestion proto_question;

            if (proto_parse_question(buffer, &proto_question) != 0) {
                continue;
            }

            client_fill_question(&proto_question, &session.current_question);
            session.last_answer.choice = '\0';
            session.last_answer.elapsed_ms = 0;
            session.has_current_question = 1;

            game_afficher_entete_question(proto_question.round_id, GAME_TOTAL_QUESTIONS, session.current_question.difficulty);
            game_afficher_question(&session.current_question);
            printf("Vous avez %d secondes pour repondre !\n\n", proto_question.timer_s);

            session.last_answer = game_attendre_reponse_avec_timer(proto_question.timer_s, session.current_question.answer_count);
            if (session.last_answer.choice != '\0') {
                proto_build_answer(buffer,
                                   sizeof(buffer),
                                   proto_question.round_id,
                                   session.last_answer.choice);
                if (proto_send_cstr(session.socket_fd, buffer) != 0) {
                    fprintf(stderr, "[CLIENT] Echec envoi reponse.\n");
                    break;
                }
            } else {
                printf("[CLIENT] Aucun appui valide, attente du resultat...\n");
            }
        } else if (code == PROTO_MSG_RESULTAT) {
            ProtoResult result;
            int est_correct;

            if (!session.has_current_question || proto_parse_result(buffer, &result) != 0) {
                continue;
            }

            session.current_question.correct_option = result.correct_option;
            est_correct = game_verifier_reponse(&session.current_question, session.last_answer.choice);
            game_afficher_resultat(est_correct, &session.current_question, session.last_answer.choice);

            session.score = result.score;
            session.alive = result.alive;
            if (!session.alive) {
                printf("[CLIENT] Vous etes elimine. Attente de la fin de partie...\n");
            }
        } else if (code == PROTO_MSG_FIN) {
            ProtoEnd end_msg;
            int est_vainqueur;

            if (proto_parse_end(buffer, &end_msg) != 0) {
                break;
            }

            session.score = end_msg.score;
            session.alive = end_msg.alive;
            est_vainqueur = strcmp(end_msg.winner, session.player_id) == 0;
            game_afficher_fin(session.score, GAME_TOTAL_QUESTIONS, est_vainqueur);
            break;
        }
    }

    if (session.hardware_ready) {
        game_cleanup_hardware();
    }

    close(session.socket_fd);
    return 0;
}
