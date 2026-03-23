/* 
 * Auteur: Bob LENGLET
 * Date: 06 Mars 2026
 * Description : Test complet des modules du jeu QPC
 * */

#include "test_complet.h"

static void convertir_question(const Question *source, DisplayedQuestion *dest) {
    game_build_question(dest,
                        source->difficulty,
                        source->question,
                        source->answers,
                        source->answer_count,
                        source->correct_option);
}

// ==========================================
//           FONCTION PRINCIPALE
// ==========================================

/**
 * Point d'entrée du programme
 */
int main(void) {
    printf("Chargement du jeu...\n\n");

    game_init_hardware();

    printf("Telechargement des questions...\n");
    if (q_init_library() != 0) {
        printf("Erreur lors du chargement des questions!\n");
        game_cleanup_hardware();
        return 1;
    }
    printf("10 questions chargees avec succes.\n\n");

    {
        int numero_question = 0;
        int score = 0;
        int partie_terminee = 0;

        while (!partie_terminee && numero_question < 10) {
            Question *question;
            DisplayedQuestion question_affichee;
            GameRoundAnswer reponse;
            int est_correct;

            numero_question++;
            question = q_pull_question();
            if (question == NULL) {
                printf("Plus de questions disponibles.\n");
                break;
            }

            convertir_question(question, &question_affichee);
            game_afficher_entete_question(numero_question, 10, question_affichee.difficulty);
            game_afficher_question(&question_affichee);

            printf("Vous avez %d secondes pour repondre !\n\n", GAME_TIMER_SECONDS);
            reponse = game_attendre_reponse_avec_timer(GAME_TIMER_SECONDS, question_affichee.answer_count);
            est_correct = game_verifier_reponse(&question_affichee, reponse.choice);
            game_afficher_resultat(est_correct, &question_affichee, reponse.choice);

            if (est_correct) {
                score++;
                printf("Bravo ! Passons a la question suivante.\n");
            } else {
                printf("Dommage ! La partie est terminee.\n");
                partie_terminee = 1;
            }
        }

        game_afficher_fin(score, 10, score == 10);
    }

    q_free_library();
    game_cleanup_hardware();
    return 0;
}
