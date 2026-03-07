/*
 * Auteur : Bob LENGLET
 * Date : 15 Février 2026
 * But : Programme testant des mélodies et le buzzer
 */

#include <stdio.h>
#include <unistd.h> // Permet d'utiliser la fonction sleep() pour les pauses
#include "biblio_buzzer.h"

int main(void) {
    
    // Initialisation du matériel
    init_buzzer();
    set_buzzer_pin(12);

    Note notes_reflexion[] = {
        {440, 150}, {0, 850}, // Tic
        {349, 150}, {0, 850}, // Tac
        {440, 150}, {0, 850}, // Tic
        {349, 150}, {0, 850}, // Tac
            // + vite
        {440, 100}, {0, 400}, 
        {349, 100}, {0, 400},
        {440, 100}, {0, 150}, 
        {349, 100}, {0, 150},
        {523, 400}         
    };
    Melodie reflexion = {notes_reflexion, sizeof(notes_reflexion)/sizeof(Note)};


    Note notes_victoire[] = {
        {523, 120}, {0, 30},  // Do
        {659, 120}, {0, 30},  // Mi
        {784, 120}, {0, 30},  // Sol
        {1046, 500}           // Do aigu
    };
    Melodie victoire = {notes_victoire, sizeof(notes_victoire)/sizeof(Note)};

    Note notes_echec[] = {
        {392, 300}, {0, 50},  // Sol
        {369, 300}, {0, 50},  // Fa dièse
        {349, 300}, {0, 50},  // Fa naturel
        {329, 900}            // Mi grave long
    };
    Melodie echec = {notes_echec, sizeof(notes_echec)/sizeof(Note)};

    Note notes_champion[] = {
        {523, 130}, {0, 20}, // Do5
        {523, 130}, {0, 20}, // Do5
        {523, 130}, {0, 20}, // Do5
        {523, 130}, {0, 20}, // Do5
        {587, 130}, {0, 20}, // Re5
        {659, 130}, {0, 20}, // Mi5
        {523, 400}, {0, 100},// Do5 (Tenu)
        
        {523, 130}, {0, 20}, // Do5
        {523, 130}, {0, 20}, // Do5
        {523, 130}, {0, 20}, // Do5
        {523, 130}, {0, 20}, // Do5
        {659, 130}, {0, 20}, // Mi5
        {587, 130}, {0, 20}, // Re5
        {523, 400}, {0, 200},// Do5 (Tenu)

        {784, 200}, {0, 50}, // Sol5
        {659, 200}, {0, 50}, // Mi5
        {1046, 600}          // Do6 final éclatant !
    };
    Melodie melodie_champion = {notes_champion, sizeof(notes_champion)/sizeof(Note)};
    
    printf("\nMusique de questions pour un champion");
    sleep(1);
    jouer_melodie(melodie_champion);

    return 0;
}