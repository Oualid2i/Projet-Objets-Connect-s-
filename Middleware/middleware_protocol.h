/**
 * @file middleware_protocol.h
 * @brief Définition du protocole texte simple utilisé entre l'hôte et les valisettes.
 */
#ifndef MIDDLEWARE_PROTOCOL_H
#define MIDDLEWARE_PROTOCOL_H

#include <stddef.h>

/** @brief Nombre maximal de joueurs connectés simultanément. */
#define MW_MAX_PLAYERS 8
/** @brief Taille maximale d'un identifiant joueur. */
#define MW_MAX_PLAYER_ID_LEN 16
/** @brief Taille maximale d'une chaîne de difficulté. */
#define MW_MAX_DIFFICULTY_LEN 32
/** @brief Taille maximale du texte d'une question. */
#define MW_MAX_QUESTION_LEN 512
/** @brief Taille maximale du texte d'une réponse. */
#define MW_MAX_ANSWER_LEN 256
/** @brief Nombre maximal de réponses proposées. */
#define MW_MAX_ANSWERS 4
/** @brief Taille maximale d'un message protocolaire complet. */
#define MW_MAX_MSG_LEN 2048
/** @brief Durée d'une manche en secondes côté protocole. */
#define MW_ROUND_DURATION_SECONDS 15

/** @brief Code de requête de connexion `JOIN`. */
#define PROTO_REQ_JOIN 100
/** @brief Code de réponse positive `JOIN_OK`. */
#define PROTO_REP_JOIN_OK 101
/** @brief Code de refus `JOIN_KO`. */
#define PROTO_REP_JOIN_KO 102
/** @brief Code d'envoi d'une question. */
#define PROTO_MSG_QUESTION 120
/** @brief Code d'envoi d'une réponse joueur. */
#define PROTO_REQ_REPONDRE 130
/** @brief Code d'annonce du résultat d'une manche. */
#define PROTO_MSG_RESULTAT 140
/** @brief Code de fin de partie. */
#define PROTO_MSG_FIN 150

/**
 * @brief Structure intermédiaire représentant un message QUESTION décodé.
 */
typedef struct {
    int round_id;                                      /**< Numéro de manche. */
    int timer_s;                                       /**< Durée du chrono en secondes. */
    char difficulty[MW_MAX_DIFFICULTY_LEN];            /**< Difficulté affichée. */
    int answer_count;                                  /**< Nombre de réponses proposées. */
    char question[MW_MAX_QUESTION_LEN];                /**< Texte de la question. */
    char answers[MW_MAX_ANSWERS][MW_MAX_ANSWER_LEN];   /**< Réponses proposées. */
} ProtoQuestion;

/**
 * @brief Structure intermédiaire représentant un message RESULTAT décodé.
 */
typedef struct {
    int round_id;           /**< Numéro de manche concerné. */
    char correct_option;    /**< Lettre de la bonne réponse. */
    int alive;              /**< Indique si le joueur reste vivant. */
    int score;              /**< Score du joueur après évaluation. */
    int remaining;          /**< Nombre de joueurs restants. */
} ProtoResult;

/**
 * @brief Structure intermédiaire représentant un message FIN décodé.
 */
typedef struct {
    char winner[MW_MAX_PLAYER_ID_LEN]; /**< Identifiant du gagnant. */
    int score;                         /**< Score final transmis. */
    int alive;                         /**< Indique si le client destinataire a survécu. */
} ProtoEnd;

/**
 * @brief Retourne un horodatage monotone en millisecondes.
 *
 * @return Nombre de millisecondes écoulées selon l'horloge utilisée.
 */
long proto_now_ms(void);

/**
 * @brief Copie une chaîne en garantissant la terminaison par `\0`.
 *
 * @param[out] dest Destination.
 * @param[in] dest_size Taille de @p dest.
 * @param[in] src Source à copier.
 */
void proto_copy_text(char *dest, size_t dest_size, const char *src);

/**
 * @brief Copie et nettoie un texte pour l'insérer dans le protocole.
 *
 * @param[out] dest Destination.
 * @param[in] dest_size Taille de @p dest.
 * @param[in] src Source à nettoyer.
 */
void proto_sanitize_text(char *dest, size_t dest_size, const char *src);

/**
 * @brief Envoie une chaîne C terminée par `\0` sur une socket.
 *
 * @param[in] fd Descripteur de socket.
 * @param[in] msg Message à envoyer.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int proto_send_cstr(int fd, const char *msg);

/**
 * @brief Reçoit une chaîne C terminée par `\0` depuis une socket.
 *
 * @param[in] fd Descripteur de socket.
 * @param[out] buffer Tampon de réception.
 * @param[in] buffer_size Taille de @p buffer.
 * @return 0 en cas de succès, une valeur négative sinon.
 */
int proto_recv_cstr(int fd, char *buffer, size_t buffer_size);

/**
 * @brief Extrait le code numérique d'un message protocolaire.
 *
 * @param[in] msg Message brut reçu.
 * @return Le code extrait, ou une valeur négative en cas d'erreur.
 */
int proto_get_code(const char *msg);

/**
 * @brief Construit un message `JOIN`.
 */
int proto_build_join(char *buffer, size_t buffer_size);

/**
 * @brief Construit un message `JOIN_OK`.
 */
int proto_build_join_ok(char *buffer, size_t buffer_size, const char *player_id);

/**
 * @brief Construit un message `JOIN_KO`.
 */
int proto_build_join_ko(char *buffer, size_t buffer_size, const char *cause);

/**
 * @brief Construit un message `REPONDRE`.
 */
int proto_build_answer(char *buffer, size_t buffer_size, int round_id, char choice);

/**
 * @brief Parse un message `JOIN`.
 */
int proto_parse_join(const char *msg, int *ready);

/**
 * @brief Parse un message `JOIN_OK`.
 */
int proto_parse_join_ok(const char *msg, char *player_id, size_t player_id_size);

/**
 * @brief Parse un message `JOIN_KO`.
 */
int proto_parse_join_ko(const char *msg, char *cause, size_t cause_size);

/**
 * @brief Parse un message `REPONDRE`.
 */
int proto_parse_answer(const char *msg, int *round_id, char *choice);

/**
 * @brief Parse un message `QUESTION`.
 */
int proto_parse_question(const char *msg, ProtoQuestion *question);

/**
 * @brief Parse un message `RESULTAT`.
 */
int proto_parse_result(const char *msg, ProtoResult *result);

/**
 * @brief Parse un message `FIN`.
 */
int proto_parse_end(const char *msg, ProtoEnd *end_msg);

#endif
