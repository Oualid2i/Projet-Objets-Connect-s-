/*
 * Auteur : Bob LENGLET
 * Date : 15 Fev 2026
 * But : En-tête de la bibliothèque pour le contrôle du buzzer du Joy-Pi
 */

#ifndef BIBLIO_BUZZER_H
#define BIBLIO_BUZZER_H

// Struct d'une note
typedef struct {
    int frequence; // Fréquence (Hz)
    int duree;     // Durée de la note (ms)
} Note;

// Struct d'une mélodie entière
typedef struct {
    Note* notes;   // tableau de notes
    int nb_notes;  // Nb notes dans la mélodie
} Melodie;

// Initialise la communication avec la broche du buzzer
void init_buzzer(void);

// Configure le pin du buzzer
void set_buzzer_pin(int pin);

// Fait sonner le buzzer de manière basique
void bip_test(void);

// Joue une mélodie passée en paramètre
void jouer_melodie(Melodie mel);

#endif