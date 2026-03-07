# Questions pour un Champion - Jeu Quiz sur Joy-It

## 📋 Description du projet

**Questions pour un Champion** est un jeu de quiz interactif développé pour le **Joy-It** (Raspberry Pi), dans le cadre d'un projet en objets connectés et compilation croisée. Le jeu reproduit l'expérience d'un quiz télévisé en utilisant tous les modules matériels disponibles sur le Joy-It.

### Auteurs
- Bob LENGLET & équipe OC
- Date: 06 Mars 2026

---

## 🎮 Fonctionnalités

### Modules matériels utilisés

Le jeu intègre **tous les modules** du Joy-It de manière cohérente:

| Module | Utilisation | Détails |
|--------|-------------|---------|
| **Afficheur 7 segments** (I2C) | Timer de réponse | Affiche le compte à rebours de 15 secondes au format `00:XX` |
| **4 Boutons** (GPIO) | Sélection des réponses | Haut(A), Droite(B), Bas(C), Gauche(D) |
| **Buzzer** (PWM) | Ambiance sonore | Générique, victoire, défaite, bips d'urgence |
| **Matrice LED 8x8** (SPI) | Feedback visuel | Coche (✓) pour bonne réponse, Croix (✗) pour erreur |
| **API OpenTDB** (Réseau) | Questions | 10 questions (3 faciles, 4 moyennes, 3 difficiles) |

### Déroulement du jeu

#### 1. **Phase d'initialisation** (≈20 secondes)
- Initialisation de tous les modules matériels
- Musique du générique "Questions pour un Champion"
- Téléchargement de 10 questions depuis l'API OpenTDB
- Affichage d'un message de confirmation

#### 2. **Phase de jeu** (max 10 questions)

Pour chaque question:

1. **Affichage de la question**
   - Numéro de la question (1/10 à 10/10)
   - Niveau de difficulté (easy, medium, hard)
   - Texte de la question
   - 2 à 4 réponses possibles selon le type de question

2. **Timer de 15 secondes**
   - Compte à rebours affiché sur le 7 segments
   - Dans les 5 dernières secondes: bips d'urgence de plus en plus rapides
   - Le joueur sélectionne sa réponse avec les boutons

3. **Validation et résultat**
   - **Bonne réponse:**
     - ✓ Coche verte sur la matrice LED
     - 🎵 Mélodie de victoire joyeuse (Do-Mi-Sol-Do)
     - Passage à la question suivante
   
   - **Mauvaise réponse ou timeout:**
     - ✗ Croix rouge sur la matrice LED
     - 🎵 Mélodie de défaite ("Sad Trombone")
     - **FIN DE PARTIE** (une seule erreur élimine le joueur)

#### 3. **Phase de fin** 
- Affichage du score final (X/10)
- **Si score = 10/10:**
  - 🏆 Message de victoire totale
  - Titre de CHAMPION
  - Générique triomphal
  
- **Sinon:**
  - Encouragements
  - Générique de fin
  
- Nettoyage des ressources et retour au système

---

## 🎵 Mélodies programmées

### Générique "Questions pour un Champion"
- **Utilisation:** Début et fin du jeu
- **Tempo:** Rapide et enjoué
- **Notes:** Séquence de Do5-Ré5-Mi5 avec finale éclatante en Do6

### Mélodie de victoire
- **Utilisation:** Bonne réponse
- **Progression:** Ascendante (Do → Mi → Sol → Do)
- **Durée:** ~1 seconde

### Mélodie de défaite
- **Utilisation:** Mauvaise réponse
- **Style:** "Sad Trombone"
- **Progression:** Descendante (Sol → Fa# → Fa → Mi grave)
- **Durée:** ~2 secondes

### Bips d'urgence
- **Utilisation:** 5 dernières secondes du timer
- **Fréquence:** 880 Hz (La5)
- **Comportement:** Accélération progressive (tempo diminue avec le temps)

---

## 🔧 Architecture technique

### Structure du code

```
Test_complet/
├── test_complet.c          # Programme principal
├── test_complet.h          # Prototypes des fonctions
├── APIquestions_fixed.c    # Version corrigée de l'API (HTTP/1.1)
└── makefile                # Compilation et déploiement
```

### Dépendances externes

**Bibliothèques Joy-It:**
- `biblio_7segments` (I2C, HT16K33)
- `biblio_buttons` (GPIO, wiringPi)
- `biblio_buzzer` (PWM, wiringPi)
- `biblio_matriceLEDS` (SPI, MAX7219)
- `APIquestions` (CURL, cJSON)

**Bibliothèques système:**
- `libwiringPi` - Contrôle GPIO/I2C/SPI
- `libcurl` - Requêtes HTTP vers OpenTDB
- `libcjson` - Parsing des réponses JSON

### Flux d'exécution

```
main()
  │
  ├─> Initialisation matériels (I2C, GPIO, SPI, PWM)
  │
  ├─> Générique + Chargement questions API
  │
  ├─> BOUCLE (max 10 questions):
  │     │
  │     ├─> Affichage question
  │     │
  │     ├─> attendre_reponse_avec_timer(15s)
  │     │     ├─> Affichage 7 segments
  │     │     ├─> Bips urgence si < 5s
  │     │     └─> Lecture boutons en continu
  │     │
  │     ├─> verifier_reponse()
  │     │
  │     ├─> afficher_resultat()
  │     │     ├─> SI CORRECT:
  │     │     │     ├─> Coche LED
  │     │     │     ├─> Mélodie victoire
  │     │     │     └─> Continue
  │     │     │
  │     │     └─> SI FAUX:
  │     │           ├─> Croix LED
  │     │           ├─> Mélodie défaite
  │     │           └─> BREAK (fin de partie)
  │     │
  │     └─> [Retour début de boucle ou sortie]
  │
  └─> Affichage score + Générique + Nettoyage
```

---

## 💻 Compilation et installation

### Prérequis sur le Raspberry Pi

⚠️ **IMPORTANT:** Ce programme **doit être compilé directement sur le Raspberry Pi** car l'API OpenTDB nécessite `libcurl` et `libcjson` qui ne sont pas disponibles dans le cross-compiler ARM.

```bash
# Installation des dépendances (une seule fois)
sudo apt-get update
sudo apt-get install -y libcurl4-openssl-dev libcjson-dev wiringpi gcc make
```

### Compilation et déploiement

Le `makefile` fournit plusieurs cibles:

#### 1. Transfert des fichiers vers le Pi
```bash
make transfer
```
- Crée l'arborescence sur le Pi
- Copie tous les fichiers sources nécessaires

#### 2. Compilation sur le Pi
```bash
make build-remote
```
- Transfère les fichiers (si nécessaire)
- Compile le programme directement sur le Pi
- Produit l'exécutable `test_complet`

#### 3. Exécution
```bash
make run
```
- Compile (si nécessaire)
- **Lance le jeu avec `sudo`** (requis pour wiringPi)

#### 4. Nettoyage
```bash
make clean         # Local
make clean-remote  # Sur le Pi
```

### Compilation manuelle (sur le Pi)

```bash
# Se connecter au Pi
ssh pi@192.168.176.213

# Aller dans le dossier
cd /home/pi/Desktop/TestModules/Test_complet

# Compiler
make

# Exécuter (nécessite sudo pour GPIO)
sudo ./test_complet
```

---

## 🎯 Détails d'implémentation

### Gestion du timer

Le timer utilise la fonction `time()` pour mesurer le temps écoulé:

```c
time_t debut = time(NULL);
while(1) {
    time_t maintenant = time(NULL);
    int secondes_restantes = duree - (int)difftime(maintenant, debut);
    
    // Mise à jour 7 segments seulement quand la seconde change
    if(secondes_restantes != derniere_seconde) {
        afficher_chrono(0, secondes_restantes);
        
        // Bips d'urgence dans les 5 dernières secondes
        if(secondes_restantes <= 5 && secondes_restantes > 0) {
            int tempo = 100 + (secondes_restantes * 50);
            // Plus le temps diminue, plus le tempo est rapide
            jouer_bip(tempo);
        }
    }
}
```

### Mapping des boutons

Les boutons physiques sont mappés vers les réponses:

| Bouton physique | Valeur retournée | Réponse |
|-----------------|------------------|---------|
| Haut (GPIO 37) | `'a'` | A |
| Droite (GPIO 35) | `'b'` | B |
| Bas (GPIO 33) | `'c'` | C |
| Gauche (GPIO 22) | `'d'` | D |

### Gestion des questions Vrai/Faux

L'API OpenTDB renvoie parfois des questions avec seulement 2 réponses (True/False). Le code vérifie dynamiquement le nombre de réponses:

```c
if(q->answer_count > 2) {
    printf("  [C] %s\n", q->answers[2]);
}
if(q->answer_count > 3) {
    printf("  [D] %s\n", q->answers[3]);
}
```

Et lors de la sélection:

```c
int index_reponse = reponse - 'A';
if(index_reponse < q->answer_count) {
    // Réponse valide
    return reponse;
}
```

### Correctif HTTP/2

La version originale de `APIquestions.c` rencontrait des erreurs HTTP/2 avec la version de curl sur Raspberry Pi. La solution dans `APIquestions_fixed.c`:

```c
// Force HTTP/1.1 pour éviter les problèmes de framing HTTP/2
curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

// Ajout de timeouts pour éviter les blocages
curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L);
curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 10L);
```

---

## 📊 Règles du jeu

### Règle d'élimination
**Une seule erreur élimine le joueur.** Ceci rend le jeu difficile mais fidèle à l'esprit des jeux télévisés.

### Temps de réponse
Chaque question dispose de **15 secondes** maximum. Ne pas répondre dans ce délai compte comme une erreur.

### Niveaux de difficulté
Les 10 questions sont réparties ainsi:
- **3 questions faciles** (easy)
- **4 questions moyennes** (medium)
- **3 questions difficiles** (hard)

L'ordre est mélangé aléatoirement par l'API OpenTDB.

### Score parfait
Répondre correctement aux **10 questions** déclenche un message de victoire spécial et le titre de "CHAMPION".

---

## 🐛 Débogage et résolution de problèmes

### Le programme ne compile pas
**Erreur:** `fatal error: curl/curl.h: No such file`
**Solution:**
```bash
sudo apt-get install libcurl4-openssl-dev libcjson-dev
```

### Erreur au lancement: "Impossible d'initialiser wiringPi"
**Cause:** Manque de permissions root
**Solution:** Toujours lancer avec `sudo`:
```bash
sudo ./test_complet
```

### L'afficheur 7 segments ne s'allume pas
**Vérifications:**
1. Vérifier la connexion I2C: `i2cdetect -y 1`
2. L'adresse 0x70 doit apparaître
3. Vérifier les câbles de connexion

### Les boutons ne répondent pas
**Cause possible:** Pull-up non activées
**Solution:** Vérifier dans `biblio_buttons.c`:
```c
pullUpDnControl(BTN_HAUT, PUD_UP);
```

### Erreur HTTP/2 framing layer
**Cause:** Incompatibilité HTTP/2 avec curl
**Solution:** Le code utilise déjà `APIquestions_fixed.c` qui force HTTP/1.1

### Les questions ne se chargent pas
**Vérifications:**
1. Connexion Internet: `ping google.com`
2. Disponibilité de l'API: `curl https://opentdb.com/api.php?amount=1`
3. Firewall/proxy

---

## 📈 Améliorations possibles

### Fonctionnalités futures
- [ ] Ajout d'un menu de difficulté (facile, moyen, difficile uniquement)
- [ ] Système de vies (3 erreurs au lieu d'une seule)
- [ ] Sauvegarde du meilleur score dans un fichier
- [ ] Mode multijoueur avec 2 buzzers
- [ ] Catégories de questions personnalisables
- [ ] Affichage du score sur le 7 segments en fin de partie
- [ ] Animation sur la matrice LED pendant le chargement

### Optimisations techniques
- [ ] Pré-chargement des questions en arrière-plan
- [ ] Cache des questions pour mode hors ligne
- [ ] Amélioration de l'anti-rebond des boutons
- [ ] Ajout de logs pour statistiques
- [ ] Support des accents/caractères spéciaux dans les questions

---

## 📚 Références

### API OpenTDB
- **Site web:** https://opentdb.com/
- **Documentation:** https://opentdb.com/api_config.php
- **Rate limiting:** 5 secondes entre chaque requête (respecté dans le code)

### Bibliothèques utilisées
- **WiringPi:** http://wiringpi.com/
- **libcurl:** https://curl.se/libcurl/
- **cJSON:** https://github.com/DaveGamble/cJSON

### Matériel Joy-It
- **HT16K33:** Contrôleur I2C pour afficheur 7 segments
- **MAX7219:** Contrôleur SPI pour matrice LED 8x8
- **Raspberry Pi GPIO:** Pinout Raspberry Pi

---

## 📄 Licence et crédits

**Projet académique** - OC OUAZZANI LENGLET  
Développé dans le cadre d'un projet en objets connectés et compilation croisée.

### Crédits
- **Questions:** OpenTDB (Open Trivia Database)
- **Mélodies:** Compositions originales inspirées de "Questions pour un Champion"
- **Bibliothèques matérielles:** Bob LENGLET (7segments, buttons, buzzer, matriceLEDS, APIquestions)

---

## 🎓 Conclusion

Ce projet démontre l'intégration complète de tous les modules du Joy-It dans une application ludique et interactive. Il illustre les concepts suivants:

✅ **Programmation bas niveau** (GPIO, I2C, SPI, PWM)  
✅ **Gestion du temps réel** (timers, interruptions)  
✅ **Networking** (API REST, HTTP, JSON)  
✅ **Architecture logicielle** (modularité, bibliothèques)  
✅ **Débogage embarqué** (cross-compilation, remote debugging)  

**Bon jeu et devenez un véritable Champion !** 🏆
