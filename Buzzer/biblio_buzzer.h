/**
 * @file biblio_buzzer.h
 * @brief API de contrôle du buzzer de la valisette.
 */

#ifndef BIBLIO_BUZZER_H
#define BIBLIO_BUZZER_H

/**
 * @brief Représente une note élémentaire.
 */
typedef struct {
    int frequence; /**< Fréquence de la note en hertz. */
    int duree;     /**< Durée de la note en millisecondes. */
} Note;

/**
 * @brief Représente une mélodie complète.
 */
typedef struct {
    Note* notes;   /**< Tableau des notes de la mélodie. */
    int nb_notes;  /**< Nombre de notes dans le tableau. */
} Melodie;

/**
 * @brief Initialise la communication avec la broche du buzzer.
 */
void init_buzzer(void);

/**
 * @brief Configure dynamiquement la broche du buzzer.
 *
 * @param[in] pin Numéro de la broche à utiliser.
 */
void set_buzzer_pin(int pin);

/**
 * @brief Déclenche un bip de test simple.
 */
void bip_test(void);

/**
 * @brief Joue une mélodie passée en paramètre.
 *
 * @param[in] mel Mélodie à jouer.
 */
void jouer_melodie(Melodie mel);

#endif
