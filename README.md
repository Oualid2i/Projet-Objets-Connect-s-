# Projet Objets Connectes

Projet de quiz "Questions pour un Champion" pour valisette EnjoyPI / Raspberry Pi 3B.

Le depot contient maintenant deux modes :
- `Test_complet/` pour le jeu solo local avec les modules materiels.
- `sa_partie/` + `game_engine/` + `Middleware/` pour le mode multi-joueurs TCP direct.

Build rapide :
- source `env.sh`
- `make -C Test_complet install-test-complet`
- `make -C Middleware`

Lancement du middleware :
- hote PC : `./sa_main <ip> <port> <nb_joueurs_attendus>`
- client valisette : `./client_valisette <ip_hote> <port_hote>`

Architecture reseau :
- `game_engine/` contient la logique de partie cote PC.
- `sa_partie/` contient le serveur de partie et le lobby.
- `Middleware/` contient le protocole texte TCP et le client valisette.
