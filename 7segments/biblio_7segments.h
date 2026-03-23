/**
 * @file biblio_7segments.h
 * @brief API de contrôle de l'afficheur 7 segments.
 */

#ifndef BIBLIO_7SEGMENTS_H
#define BIBLIO_7SEGMENTS_H

/**
 * @brief Initialise la communication I2C avec le contrôleur HT16K33.
 */
void init_7segments(void);

/**
 * @brief Affiche un chrono au format MM:SS.
 *
 * @param[in] minutes Valeur des minutes à afficher.
 * @param[in] secondes Valeur des secondes à afficher.
 */
void afficher_chrono(int minutes, int secondes);

/**
 * @brief Règle la luminosité de l'afficheur.
 *
 * @param[in] niveau Niveau de luminosité entre 0 et 15.
 */
void regler_luminosite(int niveau);

/**
 * @brief Éteint tous les segments de l'afficheur.
 */
void effacer_7segments(void);

#endif
