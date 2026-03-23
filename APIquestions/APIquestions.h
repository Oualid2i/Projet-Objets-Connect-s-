/**
 * @file APIquestions.h
 * @brief API publique de la bibliothèque de récupération des questions OpenTDB.
 */
#ifndef APIQUESTIONS_H
#define APIQUESTIONS_H

#include <stdlib.h>
#include <string.h>

/**
 * @brief Représente une question récupérée depuis l'API OpenTDB.
 */
typedef struct {
    char *category;        /**< Catégorie de la question. */
    char *difficulty;      /**< Difficulté de la question (`easy`, `medium`, `hard`). */
    char *question;        /**< Intitulé de la question. */
    char **answers;        /**< Tableau contenant toutes les réponses mélangées. */
    int answer_count;      /**< Nombre total de réponses disponibles. */
    char correct_option;   /**< Lettre de la bonne réponse (`A`, `B`, `C`, `D`, ...). */
} Question;

/**
 * @brief Initialise la bibliothèque et charge les questions depuis OpenTDB.
 *
 * @return 0 si l'initialisation a réussi, une valeur négative sinon.
 */
int q_init_library(void);

/**
 * @brief Retourne la prochaine question disponible dans la bibliothèque.
 *
 * @return Pointeur vers la prochaine question, ou `NULL` s'il n'y en a plus.
 */
Question* q_pull_question(void);

/**
 * @brief Affiche en sortie standard l'état interne de la bibliothèque pour le debug.
 */
void q_debug_dump_all(void);

/**
 * @brief Libère toute la mémoire allouée par la bibliothèque.
 */
void q_free_library(void);

#endif
