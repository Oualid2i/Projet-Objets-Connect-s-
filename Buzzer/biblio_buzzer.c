/*
 * Auteur : Bob LENGLET
 * Date : 15 fev 2026
 * But : Fonctions de la bibliothèque du buzzer du Joy-Pi
 */

#include "biblio_buzzer.h"
#include <wiringPi.h>
#include <stdio.h>

// Le buzzer du Joy-Pi est sur le GPIO 18 donc la broche 1 pour wiringPi.
#define BUZZER_PIN 1

// Variable globale pour le pin du buzzer (peut être modifiée avec set_buzzer_pin)
static int buzzer_pin = BUZZER_PIN;

void init_buzzer(void) {
    if (wiringPiSetup() == -1) {
        printf("Erreur : Impossible d'initialiser wiringPi.\n");
        return;
    }
    // Configurer la broche du buzzer en mode sortie
    pinMode(buzzer_pin, OUTPUT);
    digitalWrite(buzzer_pin, LOW); // Assure que le buzzer est éteint au départ
}

void set_buzzer_pin(int pin) {
    buzzer_pin = pin;
}

void bip_test(void) {
    printf("test bip\n");
    digitalWrite(buzzer_pin, HIGH);
    delay(500); // Reste allumé 500 millisecondes
    digitalWrite(buzzer_pin, LOW);
    delay(500);
}

// Fonction utilitaire (PWM manuel)
static void jouer_frequence(int frequence, int duree_ms) {
    long i;
    if (frequence <= 0) {
        delay(duree_ms); // freq 0 = Silence
        return;
    }
    
    // Calcul de la demi-période en microsecondes
    long demi_periode = 1000000 / (frequence * 2);
    // Calcul du nombre de cycles pour tenir la durée demandée
    long cycles = ((long)duree_ms * 1000) / (demi_periode * 2);
    
    // Boucle d'oscillation 
    for (i = 0; i < cycles; i++) {
        digitalWrite(buzzer_pin, HIGH);
        delayMicroseconds(demi_periode);
        digitalWrite(buzzer_pin, LOW);
        delayMicroseconds(demi_periode);
    }
}

void jouer_melodie(Melodie mel) {
    int i;
    printf("Lecture de la mélodie en cours...\n");
    for (i = 0; i < mel.nb_notes; i++) {
        jouer_frequence(mel.notes[i].frequence, mel.notes[i].duree);
        
        // Pr bien séprarer les notes
        digitalWrite(buzzer_pin, LOW);
        delay(50);
    }
}