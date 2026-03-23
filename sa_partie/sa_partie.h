/**
 * @file sa_partie.h
 * @brief Serveur de partie TCP côté hôte.
 */
#ifndef SA_PARTIE_H
#define SA_PARTIE_H

#include "../Middleware/middleware_protocol.h"
#include "../game_engine/game_engine.h"

/**
 * @brief Représente une connexion client active côté hôte.
 */
typedef struct {
    int used;                                  /**< Indique si l'entrée est occupée. */
    int socket_fd;                             /**< Descripteur de socket du client. */
    char player_id[MW_MAX_PLAYER_ID_LEN];      /**< Identifiant logique du joueur. */
} client_conn_t;

/**
 * @brief Contexte global du serveur de partie.
 */
typedef struct {
    int listen_fd;                             /**< Socket d'écoute TCP. */
    int expected_players;                      /**< Nombre de joueurs attendus. */
    int started;                               /**< Indique si la partie a commencé. */
    client_conn_t clients[MW_MAX_PLAYERS];     /**< Tableau des connexions clientes. */
    GameState *game;                           /**< Moteur de jeu utilisé par l'hôte. */
} sa_ctx_t;

/**
 * @brief Initialise le serveur de partie et la socket d'écoute.
 *
 * @param[out] ctx Contexte serveur à initialiser.
 * @param[in] ip Adresse IP d'écoute.
 * @param[in] port Port TCP d'écoute.
 * @param[in] expected_players Nombre de joueurs attendu dans le lobby.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int sa_init(sa_ctx_t *ctx, const char *ip, short port, int expected_players);

/**
 * @brief Gère le lobby jusqu'au lancement de la partie.
 *
 * @param[in,out] ctx Contexte serveur.
 */
void sa_lobby(sa_ctx_t *ctx);

/**
 * @brief Exécute la boucle principale d'une partie réseau.
 *
 * @param[in,out] ctx Contexte serveur.
 */
void sa_run_game(sa_ctx_t *ctx);

/**
 * @brief Ferme les sockets et libère les ressources du serveur.
 *
 * @param[in,out] ctx Contexte serveur.
 */
void sa_shutdown(sa_ctx_t *ctx);

#endif
