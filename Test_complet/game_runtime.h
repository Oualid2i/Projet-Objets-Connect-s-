#ifndef GAME_RUNTIME_H
#define GAME_RUNTIME_H

#define GAME_MAX_DIFFICULTY_LEN 32
#define GAME_MAX_QUESTION_LEN 512
#define GAME_MAX_ANSWER_LEN 256
#define GAME_MAX_ANSWERS 4
#define GAME_TOTAL_QUESTIONS 10
#define GAME_TIMER_SECONDS 15

typedef struct {
    char difficulty[GAME_MAX_DIFFICULTY_LEN];
    char question[GAME_MAX_QUESTION_LEN];
    char answers[GAME_MAX_ANSWERS][GAME_MAX_ANSWER_LEN];
    int answer_count;
    char correct_option;
} DisplayedQuestion;

typedef struct {
    char choice;
    long elapsed_ms;
} GameRoundAnswer;

#define BUZZER_PIN_PHYS 12

void game_init_hardware(void);
void game_cleanup_hardware(void);
void game_build_question(DisplayedQuestion *dest,
                         const char *difficulty,
                         const char *question,
                         char **answers,
                         int answer_count,
                         char correct_option);
void game_afficher_question(const DisplayedQuestion *q);
void game_afficher_entete_question(int current_round, int total_rounds, const char *difficulty);
GameRoundAnswer game_attendre_reponse_avec_timer(int duree_secondes, int answer_count);
int game_verifier_reponse(const DisplayedQuestion *q, char reponse_joueur);
void game_afficher_resultat(int est_correct, const DisplayedQuestion *q, char reponse_joueur);
void game_afficher_fin(int score, int total_questions, int est_vainqueur);

#endif
