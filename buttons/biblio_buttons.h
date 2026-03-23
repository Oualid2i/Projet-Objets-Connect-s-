/**
 * @file biblio_buttons.h
 * @brief API de lecture des 4 boutons de la valisette.
 */
#ifndef BIBLIO_BUTTONS_H
#define BIBLIO_BUTTONS_H

/**
 * @brief Configure les broches des boutons en entrée.
 */
void init_buttons(void);

/**
 * @brief Lit l'état courant des boutons.
 *
 * @return `a` pour haut, `b` pour droite, `c` pour bas, `d` pour gauche,
 *         ou `0` si aucun bouton n'est pressé.
 */
char lire_boutons(void);

#endif
