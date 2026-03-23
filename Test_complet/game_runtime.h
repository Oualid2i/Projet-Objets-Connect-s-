/**
 * @file game_runtime.h
 * @brief Couche d'abstraction matérielle utilisée par le jeu côté valisette.
 */
#ifndef GAME_RUNTIME_H
#define GAME_RUNTIME_H

/** @brief Taille maximale d'une chaîne de difficulté. */
#define GAME_MAX_DIFFICULTY_LEN 32
/** @brief Taille maximale du texte d'une question. */
#define GAME_MAX_QUESTION_LEN 512
/** @brief Taille maximale du texte d'une réponse. */
#define GAME_MAX_ANSWER_LEN 256
/** @brief Nombre maximal de réponses affichables. */
#define GAME_MAX_ANSWERS 4
/** @brief Nombre total de questions dans une partie. */
#define GAME_TOTAL_QUESTIONS 10
/** @brief Durée par défaut d'une manche, en secondes. */
#define GAME_TIMER_SECONDS 15

/**
 * @brief Représentation locale d'une question prête à être affichée sur la valisette.
 */
typedef struct {
    char difficulty[GAME_MAX_DIFFICULTY_LEN]; /**< Difficulté affichée. */
    char question[GAME_MAX_QUESTION_LEN];     /**< Texte de la question. */
    char answers[GAME_MAX_ANSWERS][GAME_MAX_ANSWER_LEN]; /**< Réponses proposées. */
    int answer_count;                         /**< Nombre de réponses valides. */
    char correct_option;                      /**< Bonne option associée à la question. */
} DisplayedQuestion;

/**
 * @brief Réponse capturée localement pendant une manche.
 */
typedef struct {
    char choice;      /**< Lettre choisie par le joueur, ou `0` si aucune réponse. */
    long elapsed_ms;  /**< Temps écoulé avant la réponse, en millisecondes. */
} GameRoundAnswer;

/** @brief Numéro physique de la broche utilisée pour le buzzer. */
#define BUZZER_PIN_PHYS 12

/**
 * @brief Initialise tous les périphériques matériels nécessaires au jeu.
 */
void game_init_hardware(void);

/**
 * @brief Libère proprement les ressources matérielles du jeu.
 */
void game_cleanup_hardware(void);

/**
 * @brief Construit une question affichable à partir des données du moteur.
 *
 * @param[out] dest Structure destination à remplir.
 * @param[in] difficulty Difficulté de la question.
 * @param[in] question Texte de la question.
 * @param[in] answers Tableau de réponses source.
 * @param[in] answer_count Nombre de réponses valides dans @p answers.
 * @param[in] correct_option Lettre de la bonne réponse.
 */
void game_build_question(DisplayedQuestion *dest,
                         const char *difficulty,
                         const char *question,
                         char **answers,
                         int answer_count,
                         char correct_option);

/**
 * @brief Affiche le contenu complet d'une question sur les interfaces de la valisette.
 *
 * @param[in] q Question à afficher.
 */
void game_afficher_question(const DisplayedQuestion *q);

/**
 * @brief Affiche l'en-tête d'une question (numéro de manche, total, difficulté).
 *
 * @param[in] current_round Numéro de la manche courante.
 * @param[in] total_rounds Nombre total de manches prévues.
 * @param[in] difficulty Difficulté à afficher.
 */
void game_afficher_entete_question(int current_round, int total_rounds, const char *difficulty);

/**
 * @brief Attend une réponse utilisateur tout en gérant le chrono local.
 *
 * @param[in] duree_secondes Durée du timer en secondes.
 * @param[in] answer_count Nombre de réponses possibles.
 * @return La réponse capturée et son temps de saisie.
 */
GameRoundAnswer game_attendre_reponse_avec_timer(int duree_secondes, int answer_count);

/**
 * @brief Vérifie si une réponse joueur correspond à la bonne option.
 *
 * @param[in] q Question de référence.
 * @param[in] reponse_joueur Réponse choisie par le joueur.
 * @return 1 si la réponse est correcte, 0 sinon.
 */
int game_verifier_reponse(const DisplayedQuestion *q, char reponse_joueur);

/**
 * @brief Affiche le résultat d'une manche et déclenche les effets associés.
 *
 * @param[in] est_correct Indique si la réponse est correcte.
 * @param[in] q Question de référence.
 * @param[in] reponse_joueur Réponse donnée par le joueur.
 */
void game_afficher_resultat(int est_correct, const DisplayedQuestion *q, char reponse_joueur);

/**
 * @brief Affiche l'écran de fin de partie.
 *
 * @param[in] score Score final du joueur.
 * @param[in] total_questions Nombre total de questions jouées.
 * @param[in] est_vainqueur Indique si le joueur a gagné la partie.
 */
void game_afficher_fin(int score, int total_questions, int est_vainqueur);

#endif
