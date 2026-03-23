#include "game_runtime.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

#include "../7segments/biblio_7segments.h"
#include "../buttons/biblio_buttons.h"
#include "../Buzzer/biblio_buzzer.h"
#include "../MatriceLEDS/biblio_matriceLEDS.h"


static long game_now_ms(void) {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000L) + (ts.tv_nsec / 1000000L);
}

static void game_copy_text(char *dest, size_t dest_size, const char *src) {
    if (dest == NULL || dest_size == 0) {
        return;
    }

    if (src == NULL) {
        dest[0] = '\0';
        return;
    }

    snprintf(dest, dest_size, "%s", src);
}

static Note notes_champion[] = {
    {523, 130}, {0, 20}, {523, 130}, {0, 20}, {523, 130}, {0, 20},
    {523, 130}, {0, 20}, {587, 130}, {0, 20}, {659, 130}, {0, 20},
    {523, 400}, {0, 100},
    {523, 130}, {0, 20}, {523, 130}, {0, 20}, {523, 130}, {0, 20},
    {523, 130}, {0, 20}, {659, 130}, {0, 20}, {587, 130}, {0, 20},
    {523, 400}, {0, 200},
    {784, 200}, {0, 50}, {659, 200}, {0, 50}, {1046, 600}
};

static Note notes_victoire[] = {
    {523, 120}, {0, 30}, {659, 120}, {0, 30},
    {784, 120}, {0, 30}, {1046, 500}
};

static Note notes_echec[] = {
    {392, 300}, {0, 50}, {369, 300}, {0, 50},
    {349, 300}, {0, 50}, {329, 900}
};

static void jouer_generique(void) {
    Melodie melodie = {notes_champion, (int)(sizeof(notes_champion) / sizeof(Note))};
    jouer_melodie(melodie);
}

static void jouer_victoire(void) {
    Melodie melodie = {notes_victoire, (int)(sizeof(notes_victoire) / sizeof(Note))};
    jouer_melodie(melodie);
}

static void jouer_echec(void) {
    Melodie melodie = {notes_echec, (int)(sizeof(notes_echec) / sizeof(Note))};
    jouer_melodie(melodie);
}

void game_init_hardware(void) {
    init_buttons();
    init_7segments();
    init_matrice();
    pinMode(BUZZER_PIN_PHYS, OUTPUT);
    digitalWrite(BUZZER_PIN_PHYS, LOW);
    set_buzzer_pin(BUZZER_PIN_PHYS);

    printf("Modules materiels initialises\n");
    printf("Musique du generique...\n");
    jouer_generique();
}

void game_cleanup_hardware(void) {
    clear_matrice();
    effacer_7segments();
}

void game_build_question(DisplayedQuestion *dest,
                         const char *difficulty,
                         const char *question,
                         char **answers,
                         int answer_count,
                         char correct_option) {
    int i;

    memset(dest, 0, sizeof(*dest));
    game_copy_text(dest->difficulty, sizeof(dest->difficulty), difficulty);
    game_copy_text(dest->question, sizeof(dest->question), question);
    dest->answer_count = answer_count;
    dest->correct_option = correct_option;

    for (i = 0; i < answer_count && i < GAME_MAX_ANSWERS; i++) {
        game_copy_text(dest->answers[i], sizeof(dest->answers[i]), answers[i]);
    }
}

void game_afficher_entete_question(int current_round, int total_rounds, const char *difficulty) {
    printf("   QUESTION %d/%d\n", current_round, total_rounds);
    printf("   Difficulte: %s\n", difficulty ? difficulty : "unknown");
}

void game_afficher_question(const DisplayedQuestion *q) {
    int i;

    printf("\n");
    printf("================================================\n");
    printf("QUESTION: %s\n", q->question);
    printf("================================================\n");

    for (i = 0; i < q->answer_count && i < GAME_MAX_ANSWERS; i++) {
        printf("  [%c] %s\n", 'A' + i, q->answers[i]);
    }

    printf("================================================\n");
    printf("Utilisez les boutons pour repondre:\n");
    printf("  Haut(A) / Droite(B) / Bas(C) / Gauche(D)\n");
    printf("================================================\n\n");
}

GameRoundAnswer game_attendre_reponse_avec_timer(int duree_secondes, int answer_count) {
    GameRoundAnswer result = {0, 0};
    long start_ms = game_now_ms();
    int last_displayed = -1;

    while (1) {
        long elapsed_ms = game_now_ms() - start_ms;
        int remaining = duree_secondes - (int)(elapsed_ms / 1000L);
        char bouton;
        char reponse = 0;
        int index_reponse;

        if (remaining < 0) {
            remaining = 0;
        }

        if (remaining != last_displayed) {
            afficher_chrono(0, remaining);
            last_displayed = remaining;

            if (remaining <= 5 && remaining > 0) {
                int tempo = 100 + (remaining * 50);
                digitalWrite(BUZZER_PIN_PHYS, HIGH);
                delay(tempo / 2);
                digitalWrite(BUZZER_PIN_PHYS, LOW);
            }
        }

        if (elapsed_ms >= (long)duree_secondes * 1000L) {
            printf("\nTEMPS ECOULE !\n");
            result.elapsed_ms = (long)duree_secondes * 1000L;
            return result;
        }

        bouton = lire_boutons();
        if (bouton == 'a') {
            reponse = 'A';
        } else if (bouton == 'b') {
            reponse = 'B';
        } else if (bouton == 'c') {
            reponse = 'C';
        } else if (bouton == 'd') {
            reponse = 'D';
        }

        index_reponse = reponse ? reponse - 'A' : -1;
        if (index_reponse >= 0 && index_reponse < answer_count) {
            result.choice = reponse;
            result.elapsed_ms = elapsed_ms;
            printf("\nReponse selectionnee: %c\n", reponse);
            delay(300);
            return result;
        }

        delay(50);
    }
}

int game_verifier_reponse(const DisplayedQuestion *q, char reponse_joueur) {
    if (reponse_joueur == 0) {
        return 0;
    }
    return reponse_joueur == q->correct_option;
}

void game_afficher_resultat(int est_correct, const DisplayedQuestion *q, char reponse_joueur) {
    printf("\n");
    printf("================================================\n");

    if (est_correct) {
        printf("BONNE REPONSE\n");
        printf("La reponse etait bien: %c - %s\n",
               q->correct_option,
               q->answers[q->correct_option - 'A']);
        afficher_coche();
        jouer_victoire();
    } else {
        printf("MAUVAISE REPONSE !\n");
        if (reponse_joueur != 0) {
            printf("Vous avez repondu: %c\n", reponse_joueur);
        } else {
            printf("Vous n'avez pas repondu a temps !\n");
        }
        printf("La bonne reponse etait: %c - %s\n",
               q->correct_option,
               q->answers[q->correct_option - 'A']);
        afficher_croix();
        jouer_echec();
    }

    printf("================================================\n\n");
    delay(2000);
    clear_matrice();
}

void game_afficher_fin(int score, int total_questions, int est_vainqueur) {
    effacer_7segments();

    printf("\n\n");
    printf("========================================\n");
    printf("           FIN DE LA PARTIE             \n");
    printf("========================================\n");
    printf("Score final: %d/%d questions correctes\n", score, total_questions);

    if (est_vainqueur) {
        printf("VICTOIRE !\n");
    } else {
        printf("Partie terminee.\n");
    }

    printf("Musique de fin...\n");
    jouer_generique();
    printf("Merci d'avoir joue !\n");
}
