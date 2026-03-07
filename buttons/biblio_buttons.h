/*
 * Auteur : Bob LENGLET
 * Date : 15 Février 2026
 * But : Entete de la biblio des 4 boutons
 */
#ifndef BIBLIO_BUTTONS_H
#define BIBLIO_BUTTONS_H

// Configure les broches des 4 boutons en entrée
void init_buttons(void);

// Scanne l'état des boutons et renvoie :
// 'a' (haut), 'b' (droite), 'c' (bas), 'd' (gauche), ou 0 si rien n'est pressé
char lire_boutons(void);

#endif