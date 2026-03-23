#include "middleware_protocol.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>


static int proto_tokenize(char *copy, char *tokens[], int max_tokens) {
    int count = 0;
    char *saveptr = NULL;
    char *token = strtok_r(copy, "|", &saveptr);

    while (token != NULL && count < max_tokens) {
        tokens[count++] = token;
        token = strtok_r(NULL, "|", &saveptr);
    }

    return count;
}

static int proto_match_header(const char *msg, int code, const char *verb, char *copy, size_t copy_size, char *tokens[], int *token_count) {
    *token_count = 0;
    proto_copy_text(copy, copy_size, msg);
    *token_count = proto_tokenize(copy, tokens, 16);

    if (*token_count < 2) {
        return -1;
    }

    if (atoi(tokens[0]) != code) {
        return -1;
    }

    if (strcmp(tokens[1], verb) != 0) {
        return -1;
    }

    return 0;
}

static int proto_read_prefixed_value(const char *token, const char *prefix, char *dest, size_t dest_size) {
    size_t prefix_len = strlen(prefix);

    if (strncmp(token, prefix, prefix_len) != 0) {
        return -1;
    }

    proto_copy_text(dest, dest_size, token + prefix_len);
    return 0;
}

static int proto_parse_int_value(const char *token, const char *prefix, int *value) {
    size_t prefix_len = strlen(prefix);

    if (strncmp(token, prefix, prefix_len) != 0) {
        return -1;
    }

    *value = atoi(token + prefix_len);
    return 0;
}

static int proto_send_all(int fd, const char *buffer, size_t size) {
    size_t sent = 0;

    while (sent < size) {
        ssize_t chunk = send(fd, buffer + sent, size - sent, 0);
        if (chunk <= 0) {
            return -1;
        }
        sent += (size_t)chunk;
    }

    return 0;
}

long proto_now_ms(void) {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000L) + (ts.tv_nsec / 1000000L);
}

void proto_copy_text(char *dest, size_t dest_size, const char *src) {
    if (dest == NULL || dest_size == 0) {
        return;
    }

    if (src == NULL) {
        dest[0] = '\0';
        return;
    }

    snprintf(dest, dest_size, "%s", src);
}

void proto_sanitize_text(char *dest, size_t dest_size, const char *src) {
    size_t out = 0;

    if (dest == NULL || dest_size == 0) {
        return;
    }

    if (src == NULL) {
        dest[0] = '\0';
        return;
    }

    while (*src != '\0' && out + 1 < dest_size) {
        char current = *src++;

        if (current == '|' || current == '\n' || current == '\r' || current == '\t') {
            current = ' ';
        }

        dest[out++] = current;
    }

    dest[out] = '\0';
}

int proto_send_cstr(int fd, const char *msg) {
    size_t size = strlen(msg) + 1;
    return proto_send_all(fd, msg, size);
}

int proto_recv_cstr(int fd, char *buffer, size_t buffer_size) {
    size_t offset = 0;

    if (buffer == NULL || buffer_size == 0) {
        return -1;
    }

    while (1) {
        char current = '\0';
        ssize_t received = recv(fd, &current, 1, 0);

        if (received == 0) {
            return 0;
        }

        if (received < 0) {
            return -1;
        }

        if (offset + 1 < buffer_size) {
            buffer[offset++] = current;
        }

        if (current == '\0') {
            if (offset == 0) {
                buffer[0] = '\0';
            } else if (buffer[offset - 1] != '\0') {
                buffer[offset] = '\0';
            }
            return 1;
        }
    }
}

int proto_get_code(const char *msg) {
    if (msg == NULL) {
        return -1;
    }

    return atoi(msg);
}

int proto_build_join(char *buffer, size_t buffer_size) {
    return snprintf(buffer, buffer_size, "%d|JOIN|ready=1", PROTO_REQ_JOIN);
}

int proto_build_join_ok(char *buffer, size_t buffer_size, const char *player_id) {
    char sanitized[MW_MAX_PLAYER_ID_LEN];

    proto_sanitize_text(sanitized, sizeof(sanitized), player_id);
    return snprintf(buffer, buffer_size, "%d|JOIN_OK|player=%s", PROTO_REP_JOIN_OK, sanitized);
}

int proto_build_join_ko(char *buffer, size_t buffer_size, const char *cause) {
    char sanitized[64];

    proto_sanitize_text(sanitized, sizeof(sanitized), cause);
    return snprintf(buffer, buffer_size, "%d|JOIN_KO|cause=%s", PROTO_REP_JOIN_KO, sanitized);
}

int proto_build_answer(char *buffer, size_t buffer_size, int round_id, char choice) {
    return snprintf(buffer, buffer_size, "%d|REPONDRE|round=%d|choice=%c", PROTO_REQ_REPONDRE, round_id, choice);
}

int proto_parse_join(const char *msg, int *ready) {
    char copy[MW_MAX_MSG_LEN];
    char *tokens[16];
    int token_count;

    if (ready != NULL) {
        *ready = 0;
    }

    if (proto_match_header(msg, PROTO_REQ_JOIN, "JOIN", copy, sizeof(copy), tokens, &token_count) != 0) {
        return -1;
    }

    if (token_count >= 3 && ready != NULL && strcmp(tokens[2], "ready=1") == 0) {
        *ready = 1;
    }

    return 0;
}

int proto_parse_join_ok(const char *msg, char *player_id, size_t player_id_size) {
    char copy[MW_MAX_MSG_LEN];
    char *tokens[16];
    int token_count;

    if (proto_match_header(msg, PROTO_REP_JOIN_OK, "JOIN_OK", copy, sizeof(copy), tokens, &token_count) != 0) {
        return -1;
    }

    if (token_count < 3) {
        return -1;
    }

    return proto_read_prefixed_value(tokens[2], "player=", player_id, player_id_size);
}

int proto_parse_join_ko(const char *msg, char *cause, size_t cause_size) {
    char copy[MW_MAX_MSG_LEN];
    char *tokens[16];
    int token_count;

    if (proto_match_header(msg, PROTO_REP_JOIN_KO, "JOIN_KO", copy, sizeof(copy), tokens, &token_count) != 0) {
        return -1;
    }

    if (token_count < 3) {
        return -1;
    }

    return proto_read_prefixed_value(tokens[2], "cause=", cause, cause_size);
}

int proto_parse_answer(const char *msg, int *round_id, char *choice) {
    char copy[MW_MAX_MSG_LEN];
    char *tokens[16];
    int token_count;
    char choice_text[8];

    if (round_id != NULL) {
        *round_id = 0;
    }
    if (choice != NULL) {
        *choice = '\0';
    }

    if (proto_match_header(msg, PROTO_REQ_REPONDRE, "REPONDRE", copy, sizeof(copy), tokens, &token_count) != 0) {
        return -1;
    }

    if (token_count < 4) {
        return -1;
    }

    if (proto_parse_int_value(tokens[2], "round=", round_id) != 0) {
        return -1;
    }

    if (proto_read_prefixed_value(tokens[3], "choice=", choice_text, sizeof(choice_text)) != 0) {
        return -1;
    }

    if (choice != NULL) {
        *choice = (char)toupper((unsigned char)choice_text[0]);
    }

    return 0;
}

int proto_parse_question(const char *msg, ProtoQuestion *question) {
    char copy[MW_MAX_MSG_LEN];
    char *tokens[16];
    int token_count;
    int i;

    if (question == NULL) {
        return -1;
    }

    memset(question, 0, sizeof(*question));

    if (proto_match_header(msg, PROTO_MSG_QUESTION, "QUESTION", copy, sizeof(copy), tokens, &token_count) != 0) {
        return -1;
    }

    for (i = 2; i < token_count; i++) {
        if (proto_parse_int_value(tokens[i], "round=", &question->round_id) == 0) {
            continue;
        }
        if (proto_parse_int_value(tokens[i], "timer=", &question->timer_s) == 0) {
            continue;
        }
        if (proto_parse_int_value(tokens[i], "count=", &question->answer_count) == 0) {
            continue;
        }
        if (proto_read_prefixed_value(tokens[i], "difficulty=", question->difficulty, sizeof(question->difficulty)) == 0) {
            continue;
        }
        if (proto_read_prefixed_value(tokens[i], "question=", question->question, sizeof(question->question)) == 0) {
            continue;
        }
        if (proto_read_prefixed_value(tokens[i], "A=", question->answers[0], sizeof(question->answers[0])) == 0) {
            continue;
        }
        if (proto_read_prefixed_value(tokens[i], "B=", question->answers[1], sizeof(question->answers[1])) == 0) {
            continue;
        }
        if (proto_read_prefixed_value(tokens[i], "C=", question->answers[2], sizeof(question->answers[2])) == 0) {
            continue;
        }
        if (proto_read_prefixed_value(tokens[i], "D=", question->answers[3], sizeof(question->answers[3])) == 0) {
            continue;
        }
    }

    if (question->answer_count < 2 || question->answer_count > MW_MAX_ANSWERS) {
        return -1;
    }

    return 0;
}

int proto_parse_result(const char *msg, ProtoResult *result) {
    char copy[MW_MAX_MSG_LEN];
    char *tokens[16];
    int token_count;
    char correct_text[8];
    int i;

    if (result == NULL) {
        return -1;
    }

    memset(result, 0, sizeof(*result));

    if (proto_match_header(msg, PROTO_MSG_RESULTAT, "RESULTAT", copy, sizeof(copy), tokens, &token_count) != 0) {
        return -1;
    }

    for (i = 2; i < token_count; i++) {
        if (proto_parse_int_value(tokens[i], "round=", &result->round_id) == 0) {
            continue;
        }
        if (proto_parse_int_value(tokens[i], "alive=", &result->alive) == 0) {
            continue;
        }
        if (proto_parse_int_value(tokens[i], "score=", &result->score) == 0) {
            continue;
        }
        if (proto_parse_int_value(tokens[i], "remaining=", &result->remaining) == 0) {
            continue;
        }
        if (proto_read_prefixed_value(tokens[i], "correct=", correct_text, sizeof(correct_text)) == 0) {
            result->correct_option = correct_text[0];
            continue;
        }
    }

    return 0;
}

int proto_parse_end(const char *msg, ProtoEnd *end_msg) {
    char copy[MW_MAX_MSG_LEN];
    char *tokens[16];
    int token_count;
    int i;

    if (end_msg == NULL) {
        return -1;
    }

    memset(end_msg, 0, sizeof(*end_msg));

    if (proto_match_header(msg, PROTO_MSG_FIN, "FIN", copy, sizeof(copy), tokens, &token_count) != 0) {
        return -1;
    }

    for (i = 2; i < token_count; i++) {
        if (proto_parse_int_value(tokens[i], "score=", &end_msg->score) == 0) {
            continue;
        }
        if (proto_parse_int_value(tokens[i], "alive=", &end_msg->alive) == 0) {
            continue;
        }
        if (proto_read_prefixed_value(tokens[i], "winner=", end_msg->winner, sizeof(end_msg->winner)) == 0) {
            continue;
        }
    }

    return 0;
}
