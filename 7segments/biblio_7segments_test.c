/*
 * Auteur : Bob LENGLET
 * Date : 15 Février 2026
 * But : Programme exemple d'utilisation de la bibliothèque du 7 segments
 * */

#include <stdio.h>
#include <wiringPi.h>
#include "biblio_7segments.h"

int main(void) {
    // Choisis ton temps de départ ici (entre 0 et 59)
    int n = 15; 
    
    printf("Démarrage du compte à rebours de %d secondes...\n", n);
    
    init_7segments();
    effacer_7segments();

    // Sécurité : on borne n entre 0 et 59
    if (n < 0) n = 0;
    if (n > 59) n = 59;

    // Décompte : on part de n et on descend jusqu'à 0
    for (int i = n; i >= 0; i--) {
        int m = i / 60; // Restera à 0, mais on garde le format
        int s = i % 60;
        
        printf("Temps restant : %02d:%02d\n", m, s);
        afficher_chrono(m, s);
        
        delay(1000); // Attendre 1 seconde
    }

    printf("Temps écoulé !\n");
    
    // On efface l'écran à la fin
    effacer_7segments();

    return 0;
}