#include "game_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Question *game_current_question(const GameState *game) {
    if (game == NULL) {
        return NULL;
    }

    if (game->current_question_index < 0 || game->current_question_index >= GAME_TOTAL_QUESTIONS) {
        return NULL;
    }

    return game->questions[game->current_question_index];
}

static int game_choice_is_valid(const Question *question, char response) {
    int max_choice;

    if (question == NULL) {
        return 0;
    }

    max_choice = 'A' + question->answer_count - 1;
    return response >= 'A' && response <= max_choice;
}

GameState *game_init(void) {
    GameState *game = calloc(1, sizeof(*game));
    int i;

    if (game == NULL) {
        fprintf(stderr, "[GAME] Erreur allocation GameState.\n");
        return NULL;
    }

    game->phase = GAME_STATE_WAITING;
    game->current_question_index = 0;

    if (q_init_library() != 0) {
        fprintf(stderr, "[GAME] Erreur chargement APIquestions.\n");
        free(game);
        return NULL;
    }

    for (i = 0; i < GAME_TOTAL_QUESTIONS; i++) {
        game->questions[i] = q_pull_question();
        if (game->questions[i] == NULL) {
            fprintf(stderr, "[GAME] Question %d indisponible.\n", i + 1);
            q_free_library();
            free(game);
            return NULL;
        }
    }

    printf("[GAME] %d questions chargees.\n", GAME_TOTAL_QUESTIONS);
    return game;
}

int game_add_player(GameState *game, const char *player_id) {
    PlayerState *player;

    if (game == NULL || player_id == NULL) {
        return -1;
    }

    if (game->phase != GAME_STATE_WAITING) {
        return -1;
    }

    if (game_find_player(game, player_id) != NULL) {
        return -1;
    }

    if (game->player_count >= MW_MAX_PLAYERS) {
        return -1;
    }

    player = &game->players[game->player_count++];
    memset(player, 0, sizeof(*player));
    proto_copy_text(player->player_id, sizeof(player->player_id), player_id);
    player->joined = 1;
    player->alive = 1;
    game->alive_count++;

    printf("[GAME] Joueur %s ajoute.\n", player->player_id);
    return 0;
}

int game_remove_player(GameState *game, const char *player_id) {
    int i;

    if (game == NULL || player_id == NULL) {
        return -1;
    }

    if (game->phase != GAME_STATE_WAITING) {
        return -1;
    }

    for (i = 0; i < game->player_count; i++) {
        if (strcmp(game->players[i].player_id, player_id) == 0) {
            int j;

            if (game->players[i].alive) {
                game->alive_count--;
            }

            for (j = i; j < game->player_count - 1; j++) {
                game->players[j] = game->players[j + 1];
            }

            memset(&game->players[game->player_count - 1], 0, sizeof(game->players[game->player_count - 1]));
            game->player_count--;
            return 0;
        }
    }

    return -1;
}

PlayerState *game_find_player(GameState *game, const char *player_id) {
    int i;

    if (game == NULL || player_id == NULL) {
        return NULL;
    }

    for (i = 0; i < game->player_count; i++) {
        if (strcmp(game->players[i].player_id, player_id) == 0) {
            return &game->players[i];
        }
    }

    return NULL;
}

const PlayerState *game_find_player_const(const GameState *game, const char *player_id) {
    int i;

    if (game == NULL || player_id == NULL) {
        return NULL;
    }

    for (i = 0; i < game->player_count; i++) {
        if (strcmp(game->players[i].player_id, player_id) == 0) {
            return &game->players[i];
        }
    }

    return NULL;
}

int game_start(GameState *game) {
    if (game == NULL) {
        return -1;
    }

    if (game->player_count <= 0) {
        fprintf(stderr, "[GAME] Aucun joueur pour lancer la partie.\n");
        return -1;
    }

    game->phase = GAME_STATE_RUNNING;
    game->current_question_index = 0;
    printf("[GAME] Partie lancee avec %d joueur(s).\n", game->player_count);
    return 0;
}

int game_prepare_question_msg(const GameState *game, char *buffer, size_t size) {
    Question *question = game_current_question(game);
    char difficulty[MW_MAX_DIFFICULTY_LEN];
    char statement[MW_MAX_QUESTION_LEN];
    char answers[MW_MAX_ANSWERS][MW_MAX_ANSWER_LEN];
    int i;

    if (question == NULL || buffer == NULL || size == 0) {
        return -1;
    }

    proto_sanitize_text(difficulty, sizeof(difficulty), question->difficulty);
    proto_sanitize_text(statement, sizeof(statement), question->question);

    for (i = 0; i < MW_MAX_ANSWERS; i++) {
        answers[i][0] = '\0';
    }

    for (i = 0; i < question->answer_count && i < MW_MAX_ANSWERS; i++) {
        proto_sanitize_text(answers[i], sizeof(answers[i]), question->answers[i]);
    }

    return snprintf(buffer,
                    size,
                    "%d|QUESTION|round=%d|timer=%d|difficulty=%s|count=%d|question=%s|A=%s|B=%s|C=%s|D=%s",
                    PROTO_MSG_QUESTION,
                    game->current_question_index + 1,
                    MW_ROUND_DURATION_SECONDS,
                    difficulty,
                    question->answer_count,
                    statement,
                    answers[0],
                    answers[1],
                    answers[2],
                    answers[3]);
}

int game_process_answer(GameState *game, const char *player_id, char response) {
    PlayerState *player;
    Question *question = game_current_question(game);

    if (game == NULL || player_id == NULL || question == NULL) {
        return -1;
    }

    player = game_find_player(game, player_id);
    if (player == NULL || !player->alive) {
        return -1;
    }

    if (player->answered) {
        return -1;
    }

    if (!game_choice_is_valid(question, response)) {
        return -1;
    }

    player->answered = 1;
    player->last_answer = response;
    printf("[GAME] Reponse de %s: %c\n", player->player_id, response);
    return 0;
}

int game_mark_disconnected(GameState *game, const char *player_id) {
    PlayerState *player = game_find_player(game, player_id);

    if (player == NULL) {
        return -1;
    }

    if (!player->alive) {
        return 0;
    }

    player->alive = 0;
    player->answered = 1;
    player->last_answer = '\0';
    if (game->alive_count > 0) {
        game->alive_count--;
    }

    printf("[GAME] %s deconnecte et elimine.\n", player->player_id);
    return 0;
}

int game_all_alive_answered(const GameState *game) {
    int i;

    if (game == NULL) {
        return 0;
    }

    for (i = 0; i < game->player_count; i++) {
        if (game->players[i].alive && !game->players[i].answered) {
            return 0;
        }
    }

    return 1;
}

int game_evaluate_round(GameState *game) {
    Question *question = game_current_question(game);
    int eliminated = 0;
    int i;

    if (game == NULL || question == NULL) {
        return -1;
    }

    for (i = 0; i < game->player_count; i++) {
        PlayerState *player = &game->players[i];
        int is_correct;

        if (!player->alive) {
            player->answered = 0;
            player->last_answer = '\0';
            continue;
        }

        is_correct = player->answered && player->last_answer == question->correct_option;
        if (is_correct) {
            player->score++;
        } else {
            player->alive = 0;
            if (game->alive_count > 0) {
                game->alive_count--;
            }
            eliminated++;
        }

        player->answered = 0;
        player->last_answer = '\0';
    }

    printf("[GAME] Fin manche %d: %d elimine(s), %d survivant(s).\n",
           game->current_question_index + 1,
           eliminated,
           game->alive_count);

    return eliminated;
}

int game_prepare_result_msg(const GameState *game, const char *player_id, char *buffer, size_t size) {
    const PlayerState *player;
    Question *question = game_current_question(game);

    if (game == NULL || player_id == NULL || buffer == NULL || size == 0 || question == NULL) {
        return -1;
    }

    player = game_find_player_const(game, player_id);
    if (player == NULL) {
        return -1;
    }

    return snprintf(buffer,
                    size,
                    "%d|RESULTAT|round=%d|correct=%c|alive=%d|score=%d|remaining=%d",
                    PROTO_MSG_RESULTAT,
                    game->current_question_index + 1,
                    question->correct_option,
                    player->alive,
                    player->score,
                    game->alive_count);
}

int game_prepare_end_msg(const GameState *game, const char *player_id, char *buffer, size_t size) {
    const PlayerState *player;
    const char *winner = "NONE";
    int i;

    if (game == NULL || player_id == NULL || buffer == NULL || size == 0) {
        return -1;
    }

    player = game_find_player_const(game, player_id);
    if (player == NULL) {
        return -1;
    }

    for (i = 0; i < game->player_count; i++) {
        if (game->players[i].alive) {
            winner = game->players[i].player_id;
            break;
        }
    }

    return snprintf(buffer,
                    size,
                    "%d|FIN|winner=%s|score=%d|alive=%d",
                    PROTO_MSG_FIN,
                    winner,
                    player->score,
                    player->alive);
}

int game_is_finished(const GameState *game) {
    if (game == NULL) {
        return 1;
    }

    if (game->phase != GAME_STATE_RUNNING) {
        return 0;
    }

    if (game->alive_count <= 0) {
        return 1;
    }

    /* In solo mode, the last remaining player is the active player. */
    if (game->player_count > 1 && game->alive_count == 1) {
        return 1;
    }

    if (game->current_question_index >= GAME_TOTAL_QUESTIONS - 1) {
        return 1;
    }

    return 0;
}

int game_next_question(GameState *game) {
    if (game == NULL) {
        return -1;
    }

    if (game->current_question_index + 1 >= GAME_TOTAL_QUESTIONS) {
        return -1;
    }

    game->current_question_index++;
    return 0;
}

void game_cleanup(GameState *game) {
    if (game == NULL) {
        return;
    }

    q_free_library();
    free(game);
}
