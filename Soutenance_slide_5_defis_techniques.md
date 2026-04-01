# Slide 5 — Problématiques techniques et défis rencontrés

## 1. Version slide prête à copier

**Titre**

`Problématiques techniques et défis rencontrés`

**Sous-titre**

`Nous avons dû concilier temps réel local, réseau fiable et robustesse logicielle.`

## 2. Structure recommandée de la slide

Faire une slide en **2 cartes côte à côte** :

- carte gauche : `Défi 1 — UX locale vs réseau`
- carte droite : `Défi 2 — Robustesse protocole`

Chaque carte doit contenir seulement :

- `Problème`
- `Solution`
- `Impact`

## 3. Carte 1 — UX locale vs réseau

**Titre de carte**

`Défi 1 — UX locale fluide malgré le réseau`

**Problème**

`Le chrono, les boutons et le buzzer doivent rester réactifs même en multijoueur.`

**Solution**

`La valisette garde le chrono et l’interaction locale ; l’hôte arbitre seulement le résultat.`

**Impact**

`Expérience fluide côté joueur + logique centralisée côté serveur.`

**Mot-clé technique visible**

```text
CLOCK_MONOTONIC + select()
```

**Ancrage projet**

- côté valisette : `game_attendre_reponse_avec_timer()`
- côté hôte : `select()` avec timeout

## 4. Carte 2 — Robustesse protocole

**Titre de carte**

`Défi 2 — Rendre le protocole texte robuste`

**Problème**

`Les messages réseau sont du texte libre, sensibles aux caractères parasites et aux coupures.`

**Solution**

`Nettoyage des champs, délimitation explicite des messages et élimination propre d’un client déconnecté.`

**Impact**

`Le serveur reste stable même si un client envoie mal ou se déconnecte brutalement.`

**Mot-clé technique visible**

```text
proto_sanitize_text()
```

**Ancrage projet**

- nettoyage : `proto_sanitize_text()`
- réception : `proto_recv_cstr()`
- déconnexion : `game_mark_disconnected()`

## 5. Texte minimal à afficher sur la slide

Si tu veux une version très légère, tu peux afficher seulement ceci dans les cartes :

### Carte gauche

- `Problème : garder une UX locale fluide`
- `Solution : chrono local + arbitrage hôte`
- `Impact : pas de latence visible côté joueur`

### Carte droite

- `Problème : protocole texte fragile`
- `Solution : sanitation + gestion des déconnexions`
- `Impact : serveur robuste`

## 6. Mise en forme conseillée

- deux grandes cartes avec couleur légèrement différente ;
- un petit pictogramme par carte :
  - chrono / éclair pour la carte 1 ;
  - bouclier / réseau pour la carte 2 ;
- une ligne technique discrète sous chaque carte ;
- pas de paragraphe long ;
- pas de gros bloc de code.

## 7. Ce qu’il faut éviter

- raconter l’histoire du projet ;
- afficher plusieurs bugs secondaires ;
- remettre l’architecture réseau de la slide 3 ;
- remettre la boucle d’une manche de la slide 4 ;
- montrer du code trop petit pour être lu.

## 8. Références techniques du projet

- Temps local : [game_runtime.c](/home/oualid/Bureau/OCC/Projet/Projet-Objets-Connect-s-/Test_complet/game_runtime.c)
- Robustesse réseau : [middleware_protocol.c](/home/oualid/Bureau/OCC/Projet/Projet-Objets-Connect-s-/Middleware/middleware_protocol.c)
- Boucle serveur : [sa_partie.c](/home/oualid/Bureau/OCC/Projet/Projet-Objets-Connect-s-/sa_partie/sa_partie.c)
