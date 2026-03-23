/**
 * @file biblio_matriceLEDS.h
 * @brief API de contrôle de la matrice de LEDs MAX7219.
 */

#ifndef BIBLIO_MATRICELEDS_H
#define BIBLIO_MATRICELEDS_H

/**
 * @brief Initialise la matrice via le bus SPI.
 */
void init_matrice(void);

/**
 * @brief Éteint toutes les LEDs de la matrice.
 */
void clear_matrice(void);

/**
 * @brief Affiche une croix sur la matrice.
 */
void afficher_croix(void);

/**
 * @brief Affiche une coche sur la matrice.
 */
void afficher_coche(void);

#endif
