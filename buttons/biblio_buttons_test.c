/*
 * Auteur : Bob LENGLET
 * Date : 03 Mars 2026
 * But : Programme d'exemple de la bibliothèque des 4 boutons
 */

#include <stdio.h>
#include <wiringPi.h>
#include "biblio_buttons.h"

int main(void) {
    printf("Attente d'une pression\n\n");
    
    init_buttons();
    
    char dernier_etat = 0; // Pr pas spammer l'ecran
    
    while(1) {
        char etat = lire_boutons();
        
        // On affiche le résultat que si un bouton est pressé ET que c'est une nouvelle pression
        if (etat != 0 && etat != dernier_etat) {
            printf("Bouton pressé : %c\n", etat);
            delay(200); // anti rebounce
        }
        
        dernier_etat = etat;
        delay(20); // Petite pause pr pas surcharger le proc du rpi
    }
    
    return 0;
}