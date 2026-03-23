/**
 * @file game_engine.h
 * @brief Moteur de jeu côté hôte pour les parties solo et multijoueur.
 */
#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <stddef.h>

#include "../APIquestions/APIquestions.h"
#include "../Middleware/middleware_protocol.h"

/** @brief Nombre total de questions gérées par une partie. */
#define GAME_TOTAL_QUESTIONS 10

/**
 * @brief États globaux possibles d'une partie.
 */
typedef enum {
    GAME_STATE_WAITING = 0, /**< En attente de démarrage. */
    GAME_STATE_RUNNING = 1, /**< Partie en cours. */
    GAME_STATE_FINISHED = 2 /**< Partie terminée. */
} GamePhase;

/**
 * @brief État métier d'un joueur enregistré dans la partie.
 */
typedef struct {
    char player_id[MW_MAX_PLAYER_ID_LEN]; /**< Identifiant réseau du joueur. */
    int joined;                           /**< Indique si le joueur a rejoint la partie. */
    int alive;                            /**< Indique si le joueur est encore en jeu. */
    int score;                            /**< Score courant du joueur. */
    int answered;                         /**< Indique si le joueur a déjà répondu à la manche courante. */
    char last_answer;                     /**< Dernière réponse reçue pour la manche courante. */
} PlayerState;

/**
 * @brief État global du moteur de jeu côté hôte.
 */
typedef struct {
    PlayerState players[MW_MAX_PLAYERS];          /**< Joueurs inscrits dans la partie. */
    int player_count;                             /**< Nombre total de joueurs connus. */
    int alive_count;                              /**< Nombre de joueurs encore vivants. */
    Question *questions[GAME_TOTAL_QUESTIONS];    /**< Questions chargées pour la partie. */
    int current_question_index;                   /**< Index de la question courante. */
    GamePhase phase;                              /**< Phase globale de la partie. */
} GameState;

/**
 * @brief Initialise le moteur de jeu et charge les questions.
 *
 * @return Un pointeur vers l'état du jeu, ou `NULL` en cas d'échec.
 */
GameState *game_init(void);

/**
 * @brief Ajoute un joueur au moteur de jeu.
 *
 * @param[in,out] game État du jeu.
 * @param[in] player_id Identifiant du joueur.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_add_player(GameState *game, const char *player_id);

/**
 * @brief Retire un joueur du moteur de jeu.
 *
 * @param[in,out] game État du jeu.
 * @param[in] player_id Identifiant du joueur à retirer.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_remove_player(GameState *game, const char *player_id);

/**
 * @brief Recherche un joueur modifiable par son identifiant.
 *
 * @param[in,out] game État du jeu.
 * @param[in] player_id Identifiant recherché.
 * @return Pointeur vers le joueur, ou `NULL` s'il est introuvable.
 */
PlayerState *game_find_player(GameState *game, const char *player_id);

/**
 * @brief Recherche un joueur en lecture seule par son identifiant.
 *
 * @param[in] game État du jeu.
 * @param[in] player_id Identifiant recherché.
 * @return Pointeur constant vers le joueur, ou `NULL` s'il est introuvable.
 */
const PlayerState *game_find_player_const(const GameState *game, const char *player_id);

/**
 * @brief Démarre la partie et positionne le moteur sur la première question.
 *
 * @param[in,out] game État du jeu.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_start(GameState *game);

/**
 * @brief Prépare le message protocolaire décrivant la question courante.
 *
 * @param[in] game État du jeu.
 * @param[out] buffer Tampon de sortie.
 * @param[in] size Taille de @p buffer.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_prepare_question_msg(const GameState *game, char *buffer, size_t size);

/**
 * @brief Enregistre la première réponse valide reçue pour un joueur.
 *
 * @param[in,out] game État du jeu.
 * @param[in] player_id Identifiant du joueur.
 * @param[in] response Réponse fournie.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_process_answer(GameState *game, const char *player_id, char response);

/**
 * @brief Marque un joueur comme déconnecté et hors-jeu.
 *
 * @param[in,out] game État du jeu.
 * @param[in] player_id Identifiant du joueur.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_mark_disconnected(GameState *game, const char *player_id);

/**
 * @brief Indique si tous les joueurs encore vivants ont répondu.
 *
 * @param[in] game État du jeu.
 * @return 1 si tous les joueurs vivants ont répondu, 0 sinon.
 */
int game_all_alive_answered(const GameState *game);

/**
 * @brief Évalue la manche courante et applique les éliminations éventuelles.
 *
 * @param[in,out] game État du jeu.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_evaluate_round(GameState *game);

/**
 * @brief Prépare le message résultat destiné à un joueur donné.
 *
 * @param[in] game État du jeu.
 * @param[in] player_id Identifiant du joueur destinataire.
 * @param[out] buffer Tampon de sortie.
 * @param[in] size Taille de @p buffer.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_prepare_result_msg(const GameState *game, const char *player_id, char *buffer, size_t size);

/**
 * @brief Prépare le message de fin de partie destiné à un joueur donné.
 *
 * @param[in] game État du jeu.
 * @param[in] player_id Identifiant du joueur destinataire.
 * @param[out] buffer Tampon de sortie.
 * @param[in] size Taille de @p buffer.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_prepare_end_msg(const GameState *game, const char *player_id, char *buffer, size_t size);

/**
 * @brief Indique si la partie est terminée.
 *
 * @param[in] game État du jeu.
 * @return 1 si la partie est terminée, 0 sinon.
 */
int game_is_finished(const GameState *game);

/**
 * @brief Passe à la question suivante si la partie continue.
 *
 * @param[in,out] game État du jeu.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int game_next_question(GameState *game);

/**
 * @brief Libère toutes les ressources du moteur de jeu.
 *
 * @param[in,out] game État du jeu à détruire.
 */
void game_cleanup(GameState *game);

#endif
