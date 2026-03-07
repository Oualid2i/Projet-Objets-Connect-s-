/*
 * Auteur : Bob LENGLET
 * Date : 03 Mars 2026
 * But : Entetes de la bibliothèque de gestion de la matrice de LEDs & prot SPI
 */


#ifndef BIBLIO_MATRICELEDS_H
#define BIBLIO_MATRICELEDS_H

// Initialise la matrice MAX7219 via le bus SPI
void init_matrice(void);

// Éteint toutes les LEDs
void clear_matrice(void);

// Affiche une croix
void afficher_croix(void);

// Affiche une coche
void afficher_coche(void);

#endif