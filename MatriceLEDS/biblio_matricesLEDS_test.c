/*
 * Auteur : Bob LENGLET
 * Date : 03 Mars 2026
 * But : Exemples d'utilisation de la bibliothèque de la matrice de LEDs
 */

#include <stdio.h>
#include <wiringPi.h>
#include "biblio_matriceLEDS.h"

int main(void) {
    printf("Initialisation de la matrice LED\n");
    init_matrice();
    
    printf("Affichage de la croix)\n");
    afficher_croix();
    delay(2000); // 2 sec
    
    printf("Effacement\n");
    clear_matrice();
    delay(500);
    
    printf("Affichage de la coche\n");
    afficher_coche();
    delay(2000);
    
    clear_matrice();
    printf("Fin du test\n");
    return 0;
}