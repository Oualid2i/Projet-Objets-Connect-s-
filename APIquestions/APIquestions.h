/*
 * Auteur : Bob LENGLET
 * Date : 29 Janvier 2026
 * But : entetes de la biblio pr pour fetch les questions en ligne
 * */
#ifndef APIQUESTIONS_H
#define APIQUESTIONS_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *difficulty;      // "easy", "medium", "hard"
    char *question;
    char **answers;        // Tableau contenant TOUTES les réponses mélangées
    int answer_count;      // Nombre total de réponses (4 pour QCM, 2 pour Vrai/Faux)
    char correct_option;   // La bonne réponse sous forme de lettre: 'A', 'B', 'C', 'D'...
} Question;


// Init de la librairie, fetch les questions en ligne
int q_init_library(void);

// Pointeur qui point vers la prochaine question
Question* q_pull_question(void);

// Debug qui affiche la mémoire
void q_debug_dump_all(void);

// Libère la mémoire
void q_free_library(void);

#endif