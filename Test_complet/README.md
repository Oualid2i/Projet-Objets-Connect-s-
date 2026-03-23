# Test complet du jeu

Le dossier `Test_complet/` contient la version solo du moteur de jeu sur une valisette EnjoyPI.

## Comportement
- Chargement de 10 questions OpenTDB.
- Timer unique partage a **15 secondes** par question.
- Utilisation conjointe du 7 segments, des boutons, de la matrice LED et du buzzer.
- Une erreur ou un timeout met fin a la partie.

## Build
- Exporter `PATH_CCC` avec `source ../env.sh`.
- Pour une nouvelle valisette, installer d'abord les dependances systeme avec `make PI_IP=IP_DE_LA_VALISETTE prepare-remote`.
- Compiler localement avec `make build-cross`.
- Deployer et linker sur la valisette avec `make PI_IP=IP_DE_LA_VALISETTE install-test-complet`.
- Le linkage final de `APIquestions.c` se fait sur le Raspberry Pi, car cette partie depend de `libcurl` et `libcjson`.
- La bibliotheque `libwiringPi.so` est fournie par le depot, copitee sur la valisette puis installee dans `/usr/local/lib` pendant `install-test-complet`.
- Par defaut, le makefile utilise `pi` / `raspberry`.
- Si vous utilisez un autre mot de passe SSH, vous pouvez ajouter `PI_PASS=...`.
- Si vous utilisez deja une cle SSH, `sshpass` n'est pas necessaire.

## Cibles make utiles
- `build-api`
- `build-cross`
- `link-remote`
- `prepare-remote`
- `install-test-complet`
- `run-remote`
- `clean`

## Notes
- Le runtime materiel partage est maintenant dans `game_runtime.c`.
- Ce meme runtime est reutilise par `client_valisette` dans le mode multi-joueurs.
