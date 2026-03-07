/*
 * Auteur : Bob LENGLET
 * Date : 15 Février 2026
 * But : Entete de la bibliothèque de gestion de l'affichage 7 segments
 */

#ifndef BIBLIO_7SEGMENTS_H
#define BIBLIO_7SEGMENTS_H

// Initialise la communication I2C avec le contrôleur HT16K33 (adresse 0x70)
void init_7segments(void);

// Affiche le temps au format MM:SS 
void afficher_chrono(int minutes, int secondes);

// Règle la luminosité de l'afficheur de 0 à 15
void regler_luminosite(int niveau);

// Éteint ts les segments de l'afficheur
void effacer_7segments(void);

#endif