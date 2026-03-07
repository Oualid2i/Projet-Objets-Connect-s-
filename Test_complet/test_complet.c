/* 
 * Auteur: Bob LENGLET
 * Date: 06 Mars 2026
 * Description : Test complet des modules du jeu QPC
 * */

#include "test_complet.h"

/**
 * Mélodie du générique 
 */
Note notes_champion[] = {
    {523, 130}, {0, 20}, {523, 130}, {0, 20}, {523, 130}, {0, 20},
    {523, 130}, {0, 20}, {587, 130}, {0, 20}, {659, 130}, {0, 20},
    {523, 400}, {0, 100},
    {523, 130}, {0, 20}, {523, 130}, {0, 20}, {523, 130}, {0, 20},
    {523, 130}, {0, 20}, {659, 130}, {0, 20}, {587, 130}, {0, 20},
    {523, 400}, {0, 200},
    {784, 200}, {0, 50}, {659, 200}, {0, 50}, {1046, 600}
};

/**
 * Mélodie de victoire 
 */
Note notes_victoire[] = {
    {523, 120}, {0, 30}, {659, 120}, {0, 30},
    {784, 120}, {0, 30}, {1046, 500}
};

/**
 * Mélodie de défaite
 */
Note notes_echec[] = {
    {392, 300}, {0, 50}, {369, 300}, {0, 50},
    {349, 300}, {0, 50}, {329, 900}
};

// Tailles des tableaux de mélodies
const int nb_notes_champion = sizeof(notes_champion) / sizeof(Note);
const int nb_notes_victoire = sizeof(notes_victoire) / sizeof(Note);
const int nb_notes_echec = sizeof(notes_echec) / sizeof(Note);

/**
 * Fonction pour les bips d'urgence
 */
void buzzer_urgence(int tempo_ms) {
    // Bips directs via GPIO en mode physique
    for(int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN_PHYS, HIGH);
        delay(tempo_ms / 2);
        digitalWrite(BUZZER_PIN_PHYS, LOW);
        delay(tempo_ms / 2);
    }
}

// ==========================================
//        FONCTIONS DU JEU
// ==========================================

/**
 * Affiche une question et ses réponses de manière formatée
 * @param q Pointeur vers la structure Question à afficher
 */
void afficher_question(Question *q) {
    printf("\n");
    printf("================================================\n");
    printf("QUESTION: %s\n", q->question);
    printf("================================================\n");
    printf("  [A] %s\n", q->answers[0]);
    printf("  [B] %s\n", q->answers[1]);
    // Affichage conditionnel pour les questions vrai/faux (2 réponses)
    if(q->answer_count > 2) {
        printf("  [C] %s\n", q->answers[2]);
    }
    if(q->answer_count > 3) {
        printf("  [D] %s\n", q->answers[3]);
    }
    printf("================================================\n");
    printf("Utilisez les boutons pour répondre:\n");
    printf("  Haut(A) / Droite(B) / Bas(C) / Gauche(D)\n");
    printf("================================================\n\n");
}

/**
 * Gère le timer de réponse avec affichage 7 segments et buzzer d'urgence
 * 
 * - Retourne la réponse sélectionnée ou 0 si timeout
 * 
 * @param duree_secondes Durée du timer
 * @param q Pointeur vers la question (pour valider que la réponse existe)
 * @return Caractère de la réponse ('A', 'B', 'C', 'D') ou 0 si timeout
 */
char attendre_reponse_avec_timer(int duree_secondes, Question *q) {
    time_t debut = time(NULL);
    time_t maintenant;
    int secondes_restantes;
    int derniere_seconde = duree_secondes;
    char reponse = 0;
    
    while(1) {
        maintenant = time(NULL);
        secondes_restantes = duree_secondes - (int)difftime(maintenant, debut);
        
        // Timer écoulé
        if(secondes_restantes <= 0) {
            afficher_chrono(0, 0);
            printf("\nTEMPS ÉCOULÉ!\n");
            return 0; // Pas de réponse
        }
        
        // Mise à jour de l'affichage 7 segments (seulement quand la seconde change)
        if(secondes_restantes != derniere_seconde) {
            afficher_chrono(0, secondes_restantes);
            derniere_seconde = secondes_restantes;
            
            // Buzzer d'urgence dans les dernières secondes
            // Le tempo augmente à mesure que le temps diminue
            if(secondes_restantes <= 5 && secondes_restantes > 0) {
                int tempo = 100 + (secondes_restantes * 50);
                // Bip simple en mode physique
                digitalWrite(BUZZER_PIN_PHYS, HIGH);
                delay(tempo / 2);
                digitalWrite(BUZZER_PIN_PHYS, LOW);
            }
        }
        
        // Lecture des boutons en continu
        char bouton = lire_boutons();
        if(bouton != 0) {
            // Mapping boutons vers réponses
            // Haut='a'->A, Droite='b'->B, Bas='c'->C, Gauche='d'->D
            if(bouton == 'a') reponse = 'A';
            else if(bouton == 'b') reponse = 'B';
            else if(bouton == 'c') reponse = 'C';
            else if(bouton == 'd') reponse = 'D';
            
            // Vérifier que la réponse existe pour cette question
            int index_reponse = reponse - 'A';
            if(index_reponse < q->answer_count) {
                printf("\nRéponse sélectionnée: %c\n", reponse);
                delay(300); // Anti-rebond pour éviter les doubles appuis
                return reponse;
            }
        }
        
        delay(50); // Petite pause pour ne pas surcharger le CPU
    }
}

/**
 * Vérifie si la réponse du joueur est correcte
 * @param q Pointeur vers la question
 * @param reponse_joueur Réponse du joueur ('A', 'B', 'C', 'D') ou 0
 * @return 1 si correct, 0 sinon
 */
int verifier_reponse(Question *q, char reponse_joueur) {
    if(reponse_joueur == 0) {
        return 0; // Pas de réponse = faux
    }
    return (reponse_joueur == q->correct_option);
}

/**
 * Affiche le résultat d'une question avec effets visuels et sonores
 * 
 * Bonne réponse:
 * - Affiche une coche sur la matrice LED
 * - Joue la mélodie de victoire
 * 
 * Mauvaise réponse:
 * - Affiche une croix sur la matrice LED
 * - Joue la mélodie de défaite
 * 
 * @param est_correct 1 si la réponse est correcte, 0 sinon
 * @param q Pointeur vers la question
 * @param reponse_joueur Réponse donnée par le joueur
 */
void afficher_resultat(int est_correct, Question *q, char reponse_joueur) {
    printf("\n");
    printf("================================================\n");
    
    if(est_correct) {
        printf("BONNE RÉPONSE\n");
        printf("La réponse était bien: %c - %s\n", 
               q->correct_option, 
               q->answers[q->correct_option - 'A']);
        
        // Affichage coche + musique victoire
        afficher_coche();
        Melodie mel_victoire = {notes_victoire, sizeof(notes_victoire)/sizeof(Note)};
        jouer_melodie(mel_victoire);
    } else {
        printf("MAUVAISE RÉPONSE !\n");
        if(reponse_joueur != 0) {
            printf("Vous avez répondu: %c\n", reponse_joueur);
        } else {
            printf("Vous n'avez pas répondu à temps!\n");
        }
        printf("La bonne réponse était: %c - %s\n", 
               q->correct_option, 
               q->answers[q->correct_option - 'A']);
        
        // Affichage croix + musique défaite
        afficher_croix();
        Melodie mel_echec = {notes_echec, sizeof(notes_echec)/sizeof(Note)};
        jouer_melodie(mel_echec);
    }
    
    printf("================================================\n\n");
    delay(2000); // Pause pour voir le résultat
    clear_matrice(); // Nettoyage de la matrice LED
}

// ==========================================
//           FONCTION PRINCIPALE
// ==========================================

/**
 * Point d'entrée du programme
 */
int main(void) {
    printf("\n");
    
    printf("Chargement du jeu...\n\n");

    init_buttons();      // GPIO pour les 4 boutons (appelle wiringPiSetupPhys)
    init_7segments();    // I2C pour l'afficheur
    init_matrice();      // SPI pour la matrice LED
    
    // Configuration du buzzer en mode physique (pin 12 = GPIO 18)
    pinMode(BUZZER_PIN_PHYS, OUTPUT);
    digitalWrite(BUZZER_PIN_PHYS, LOW);
    set_buzzer_pin(BUZZER_PIN_PHYS); // Informer la bibliothèque du pin à utiliser
    
    printf("Modules matériels initialisés\n");
    
    // Jouer le générique pendant le chargement pour créer l'ambiance
    Melodie melodie_champion = {notes_champion, sizeof(notes_champion)/sizeof(Note)};
    printf("Musique du générique...\n");
    jouer_melodie(melodie_champion);
    
    // Chargement des questions depuis l'API OpenTDB
    // Récupère 10 questions: 3 faciles, 4 moyennes, 3 difficiles
    printf("Téléchargement des questions...\n");
    if(q_init_library() != 0) {
        printf("Erreur lors du chargement des questions!\n");
        return 1;
    }
    printf("✓ 10 questions chargées avec succès!\n\n");
    
    delay(1000);
    
    int numero_question = 0;    // Compteur de questions 
    int score = 0;              // Nombre de bonnes réponses
    int partie_terminee = 0;    // Flag pour sortir de la boucle
    
    // Boucle principale: continue tant qu'il y a des questions et pas d'erreur
    while(!partie_terminee && numero_question < 10) {
        numero_question++;
        
        // Récupérer la prochaine question
        Question *q = q_pull_question();
        if(q == NULL) {
            printf("⚠️ Plus de questions disponibles.\n");
            break;
        }
        
        // Affichage de l'en-tête de la question
        printf("   QUESTION %d/10                             \n", numero_question);
        printf("   Difficulté: %-30s│\n", q->difficulty);
        
        // Afficher la question et ses réponses
        afficher_question(q);
        
        printf("Vous avez 25 secondes pour répondre!\n\n");
        char reponse = attendre_reponse_avec_timer(25, q);
        
        // Vérifier la réponse
        int est_correct = verifier_reponse(q, reponse);
        
        // Afficher le résultat avec effets visuels et sonores
        afficher_resultat(est_correct, q, reponse);
        
        // Gestion du score et continuation du jeu
        if(est_correct) {
            score++;
            printf("Bravo! Passons à la question suivante!\n");
            delay(1500);
        } else {
            // Une seule erreur met fin à la partie (règle du jeu)
            printf("Dommage! La partie est terminée.\n");
            partie_terminee = 1;
        }
    }

    
    effacer_7segments();
    
    printf("\n\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║           FIN DE LA PARTIE                 ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n");
    printf("📊 Score final: %d/10 questions correctes\n", score);
    printf("\n");
    
    // Message de fin selon le score
    if(score == 10) {
        printf(" VICTOIRE !\n");
        jouer_melodie(melodie_champion);
    } else {
        printf("Musique de fin...\n");
        jouer_melodie(melodie_champion);
    }
    
    printf("\n");
    printf("Merci d'avoir joué!\n");
    printf("\n");
    
    // Nettoyage des ressources
    q_free_library();    // Libération de la mémoire des questions
    clear_matrice();     // Extinction de la matrice LED
    effacer_7segments(); // Extinction du 7 segments
    
    return 0;
}
