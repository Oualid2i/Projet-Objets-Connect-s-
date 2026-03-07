/**
 *	\file		main.c
 *	\brief		programme de test de la bibliotheque de questions
 *	\author		Bob Lenglet
 *	\date		29 janvier 2026
 *	\version	1.0
 */

// Compiler avec : gcc main.c -L. -lapiquestions -lcurl -lcjson -o testquestions

// main.c// main.c
#include <stdio.h>
#include "APIquestions.h"

int main() {
    if (q_init_library() != 0) { // On vérifie que l'initialisationa  réussi
        return 1;
    }

    q_debug_dump_all(); // Voir les 10 questions téléchargées

    Question *q = q_pull_question();
    if(q) {
        printf("\nHere is your question : %s\n", q->question);
    }

    if (q != NULL) {
    printf("\n=== DEBUG: STRUCT DUMP (Addresse: %p) ===\n", (void*)q);
    printf(" .category       : \"%s\"\n", q->category);
    printf(" .difficulty     : \"%s\"\n", q->difficulty);
    printf(" .question       : \"%s\"\n", q->question);
    printf(" .answer_count   : %d\n", q->answer_count);
    printf(" .correct_option : '%c'\n", q->correct_option);
    printf(" .answers (ptr)  : %p\n", (void*)q->answers);

    for (int i = 0; i < q->answer_count; i++) {
        // Marqueur visuel pour identifier la réponse stockée comme correcte
        char *marker = ('A' + i == q->correct_option) ? " <--- [CORRECT]" : "";
        
        printf("    [%d] (Lettre %c) : \"%s\"%s\n", 
               i, 'A' + i, q->answers[i], marker);
    }
    printf("===========================================\n");
}

    q_free_library();
    return 0;
}