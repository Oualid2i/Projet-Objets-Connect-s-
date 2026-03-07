/*
 * Auteur : Bob LENGLET
 * Date : 03 Mars 2026
 * But : Fonctions de la bibliothèque des 4 boutons
 */

#include <wiringPi.h>
#include "biblio_buttons.h"

// Utilisation des numéros physiques trouvés via les script python du joy it
#define BTN_HAUT   37
#define BTN_DROITE 35
#define BTN_BAS    33
#define BTN_GAUCHE 22

void init_buttons(void) {
    // initialisation de WiringPi en mode physique
    wiringPiSetupPhys(); 
    
    // Broches en mode ENTRÉE
    pinMode(BTN_HAUT, INPUT);
    pinMode(BTN_DROITE, INPUT);
    pinMode(BTN_BAS, INPUT);
    pinMode(BTN_GAUCHE, INPUT);
    
    // Activation des résistances
    // Le signal est maintenu à l'état haut quand on ne touche à rien
    pullUpDnControl(BTN_HAUT, PUD_UP);
    pullUpDnControl(BTN_DROITE, PUD_UP);
    pullUpDnControl(BTN_BAS, PUD_UP);
    pullUpDnControl(BTN_GAUCHE, PUD_UP);
}

char lire_boutons(void) {
    // Si la lecture passe à l'état bas alors un bouton a ete pressé
    if (digitalRead(BTN_HAUT) == LOW)   return 'a';
    if (digitalRead(BTN_DROITE) == LOW) return 'b';
    if (digitalRead(BTN_BAS) == LOW)    return 'c';
    if (digitalRead(BTN_GAUCHE) == LOW) return 'd';
    
    return 0; // Aucun bouton n'est enfoncé
}