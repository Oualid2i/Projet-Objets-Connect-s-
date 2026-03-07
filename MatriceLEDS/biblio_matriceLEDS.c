/*
 * Auteur : Bob LENGLET
 * Date : 03 Mars 2026
 * But : Fonctions de la bibliothèque de gestion de la matrice de LEDs & prot SPI
 */

#include <wiringPiSPI.h> // Protocole SPI
#include <stdint.h>
#include "biblio_matriceLEDS.h"

// Le script Python utilise device=1
#define SPI_CHANNEL 1 
#define SPI_SPEED 500000

// Dessin de la croix (1 = LED allumée)
static const uint8_t image_croix[8] = {
    0x81, // 10000001
    0x42, // 01000010
    0x24, // 00100100
    0x18, // 00011000
    0x18, // 00011000
    0x24, // 00100100
    0x42, // 01000010
    0x81  // 10000001
};

// Dessin de la coche (V de validation)
static const uint8_t image_coche[8] = {
    0x00, // 00000000
    0x01, // 00000001
    0x02, // 00000010
    0x04, // 00000100
    0x88, // 10001000
    0x50, // 01010000
    0x20, // 00100000
    0x00  // 00000000
};

// Fonction interne pour envoyer une commande au MAX7219
static void write_max7219(uint8_t reg, uint8_t data) {
    uint8_t buffer[2];
    buffer[0] = reg;
    buffer[1] = data;
    wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2);
}

// Fonction interne pour afficher un tableau de 8 octets
static void dessiner_image(const uint8_t *image) {
    for(int i = 0; i < 8; i++) {
        write_max7219(i + 1, image[i]);
    }
}

void init_matrice(void) {
    // Initialisation du SPI sur le canal 1
    wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED);
    
    // Configuration de la puce
    write_max7219(0x09, 0x00); // Pas de décodage BCD on fait brute
    write_max7219(0x0A, 0x03); // Luminosité
    write_max7219(0x0B, 0x07); // afficher ttes les lignes
    write_max7219(0x0C, 0x01); // Mode normal
    write_max7219(0x0F, 0x00); // Mode test désactivé
    
    clear_matrice();
}

void clear_matrice(void) {
    for(int i = 1; i <= 8; i++) {
        write_max7219(i, 0x00); // TT éteint
    }
}

void afficher_croix(void) {
    dessiner_image(image_croix);
}

void afficher_coche(void) {
    dessiner_image(image_coche);
}