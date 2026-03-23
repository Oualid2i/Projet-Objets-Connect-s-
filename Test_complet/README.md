# Test complet du jeu

Le dossier `Test_complet/` contient la version solo du moteur de jeu sur une valisette EnjoyPI.

## Comportement
- Chargement de 10 questions OpenTDB.
- Timer unique partage a **15 secondes** par question.
- Utilisation conjointe du 7 segments, des boutons, de la matrice LED et du buzzer.
- Une erreur ou un timeout met fin a la partie.

## Build
- Exporter `PATH_CCC` avec `source ../env.sh`.
- Compiler et deployer avec `make install-test-complet`.
- Le linkage final de `APIquestions.c` se fait sur le Raspberry Pi, car cette partie depend de `libcurl` et `libcjson`.

## Cibles make utiles
- `build-api`
- `build-cross`
- `link-remote`
- `install-test-complet`
- `clean`

## Notes
- Le runtime materiel partage est maintenant dans `game_runtime.c`.
- Ce meme runtime est reutilise par `client_valisette` dans le mode multi-joueurs.
