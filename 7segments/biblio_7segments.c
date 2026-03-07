/*
 * Auteur : Bob LENGLET
 * Date : 15 Février 2026
 * But : Fonctions de la bibliothèque de gestion du 7 segments
 */

#include <wiringPiI2C.h> // lib I2C
#include <stdio.h>
#include "biblio_7segments.h"

int fd_i2c; // La connexion

// Table de conversion pour le HT16K33
static const unsigned char digit_map[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
}; 

void init_7segments(void) {
    // Initialise l'I2C sur l'adresse 0x70
    fd_i2c = wiringPiI2CSetup(0x70);
    
    if (fd_i2c == -1) {
        printf("Erreur : Impossible de joindre l'afficheur sur 0x70\n");
        return;
    }

    wiringPiI2CWrite(fd_i2c, 0x21); // Activer l'oscillateur système
    wiringPiI2CWrite(fd_i2c, 0x81); // Activer l'affichage, pas de clignotement
    regler_luminosite(10);
}

void regler_luminosite(int niveau) {
    if (niveau > 15) niveau = 15;
    wiringPiI2CWrite(fd_i2c, 0xE0 | niveau); // Pour écrire directement la luminosité demandée
}

void afficher_chrono(int min, int sec) {
    // Registres  0x00, 0x02, 0x04 (:), 0x06, 0x08
    wiringPiI2CWriteReg8(fd_i2c, 0x00, digit_map[min / 10]);
    wiringPiI2CWriteReg8(fd_i2c, 0x02, digit_map[min % 10]);
    
    // Les deux points
    wiringPiI2CWriteReg8(fd_i2c, 0x04, 0x02);
    
    wiringPiI2CWriteReg8(fd_i2c, 0x06, digit_map[sec / 10]);
    wiringPiI2CWriteReg8(fd_i2c, 0x08, digit_map[sec % 10]);
}

void effacer_7segments(void) {
    for (int i = 0; i < 10; i += 2) {
        wiringPiI2CWriteReg8(fd_i2c, i, 0x00);
    }
}