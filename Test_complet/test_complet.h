/*
 * Auteur : Bob LENGLET
 * Date : 06 Mars 2026
 * But : Entete pour le test complet des modules du jeu QPC
 */

#ifndef TEST_COMPLET_H
#define TEST_COMPLET_H

// ==========================================
//            INCLUDES
// ==========================================

// biblio sys
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wiringPi.h>

// biblio locales
#include "../APIquestions/APIquestions.h"
#include "../7segments/biblio_7segments.h"
#include "../buttons/biblio_buttons.h"
#include "../Buzzer/biblio_buzzer.h"
#include "../MatriceLEDS/biblio_matriceLEDS.h"

// ==========================================
//            CONSTANTES
// ==========================================

// GPIO 18 (BCM) = Pin physique 12
#define BUZZER_PIN_PHYS 12

// ==========================================
//        DÉCLARATIONS EXTERNES
// ==========================================

// Tableaux de notes
extern Note notes_champion[];
extern Note notes_victoire[];
extern Note notes_echec[];

// lenght des tableaux de mélodies
extern const int nb_notes_champion;
extern const int nb_notes_victoire;
extern const int nb_notes_echec;

// ==========================================
//            PROTOTYPES
// ==========================================
void afficher_question(Question *q);
char attendre_reponse_avec_timer(int duree_secondes, Question *q);
int verifier_reponse(Question *q, char reponse_joueur);
void afficher_resultat(int est_correct, Question *q, char reponse_joueur);
void buzzer_urgence(int tempo_ms);

#endif
