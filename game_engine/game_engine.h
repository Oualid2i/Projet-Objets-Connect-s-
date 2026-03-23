#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <stddef.h>

#include "../APIquestions/APIquestions.h"
#include "../Middleware/middleware_protocol.h"

#define GAME_TOTAL_QUESTIONS 10

typedef enum {
    GAME_STATE_WAITING = 0,
    GAME_STATE_RUNNING = 1,
    GAME_STATE_FINISHED = 2
} GamePhase;

typedef struct {
    char player_id[MW_MAX_PLAYER_ID_LEN];
    int joined;
    int alive;
    int score;
    int answered;
    char last_answer;
} PlayerState;

typedef struct {
    PlayerState players[MW_MAX_PLAYERS];
    int player_count;
    int alive_count;
    Question *questions[GAME_TOTAL_QUESTIONS];
    int current_question_index;
    GamePhase phase;
} GameState;

GameState *game_init(void);
int game_add_player(GameState *game, const char *player_id);
int game_remove_player(GameState *game, const char *player_id);
PlayerState *game_find_player(GameState *game, const char *player_id);
const PlayerState *game_find_player_const(const GameState *game, const char *player_id);
int game_start(GameState *game);
int game_prepare_question_msg(const GameState *game, char *buffer, size_t size);
int game_process_answer(GameState *game, const char *player_id, char response);
int game_mark_disconnected(GameState *game, const char *player_id);
int game_all_alive_answered(const GameState *game);
int game_evaluate_round(GameState *game);
int game_prepare_result_msg(const GameState *game, const char *player_id, char *buffer, size_t size);
int game_prepare_end_msg(const GameState *game, const char *player_id, char *buffer, size_t size);
int game_is_finished(const GameState *game);
int game_next_question(GameState *game);
void game_cleanup(GameState *game);

#endif
