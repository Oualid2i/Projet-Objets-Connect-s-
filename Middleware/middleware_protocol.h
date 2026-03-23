#ifndef MIDDLEWARE_PROTOCOL_H
#define MIDDLEWARE_PROTOCOL_H

#include <stddef.h>

#define MW_MAX_PLAYERS 8
#define MW_MAX_PLAYER_ID_LEN 16
#define MW_MAX_DIFFICULTY_LEN 32
#define MW_MAX_QUESTION_LEN 512
#define MW_MAX_ANSWER_LEN 256
#define MW_MAX_ANSWERS 4
#define MW_MAX_MSG_LEN 2048
#define MW_ROUND_DURATION_SECONDS 15

#define PROTO_REQ_JOIN 100
#define PROTO_REP_JOIN_OK 101
#define PROTO_REP_JOIN_KO 102
#define PROTO_MSG_QUESTION 120
#define PROTO_REQ_REPONDRE 130
#define PROTO_MSG_RESULTAT 140
#define PROTO_MSG_FIN 150

typedef struct {
    int round_id;
    int timer_s;
    char difficulty[MW_MAX_DIFFICULTY_LEN];
    int answer_count;
    char question[MW_MAX_QUESTION_LEN];
    char answers[MW_MAX_ANSWERS][MW_MAX_ANSWER_LEN];
} ProtoQuestion;

typedef struct {
    int round_id;
    char correct_option;
    int alive;
    int score;
    int remaining;
} ProtoResult;

typedef struct {
    char winner[MW_MAX_PLAYER_ID_LEN];
    int score;
    int alive;
} ProtoEnd;

long proto_now_ms(void);
void proto_copy_text(char *dest, size_t dest_size, const char *src);
void proto_sanitize_text(char *dest, size_t dest_size, const char *src);

int proto_send_cstr(int fd, const char *msg);
int proto_recv_cstr(int fd, char *buffer, size_t buffer_size);
int proto_get_code(const char *msg);

int proto_build_join(char *buffer, size_t buffer_size);
int proto_build_join_ok(char *buffer, size_t buffer_size, const char *player_id);
int proto_build_join_ko(char *buffer, size_t buffer_size, const char *cause);
int proto_build_answer(char *buffer, size_t buffer_size, int round_id, char choice);

int proto_parse_join(const char *msg, int *ready);
int proto_parse_join_ok(const char *msg, char *player_id, size_t player_id_size);
int proto_parse_join_ko(const char *msg, char *cause, size_t cause_size);
int proto_parse_answer(const char *msg, int *round_id, char *choice);
int proto_parse_question(const char *msg, ProtoQuestion *question);
int proto_parse_result(const char *msg, ProtoResult *result);
int proto_parse_end(const char *msg, ProtoEnd *end_msg);

#endif
