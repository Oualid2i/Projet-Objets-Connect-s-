PATH_CC = /home/bob/Desktop/OC_OUAZZANI_LENGLET/devProjet/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
PATH_CCC = $(PATH_CC)/arm-linux-gnueabihf-gcc

IPRPI ?= 192.168.176.213

all: test_buttons

test_buttons: biblio_buttons.c biblio_buttons_test.c
	$(PATH_CCC) -std=gnu99 -D'TARGET="PI"' $^ -o $@ -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
# -std=gnu99                : Force le standard C99 avec extensions GNU (à gargder car Linaro est ancien) 
# 		(j'ai eu des problèmes de compilation et cette solution a marché)
# -I.                       : Cherche les fichiers d'en-tête dans le dossier courant
# -L.                       : Cherche les bibliothèques dans le dossier courant
# -lwiringPi                : Lie la librairie WiringPi
# -Wl,--allow-shlib-undefined : Demande au linker d'ignorer les symboles non trouvés dans les bibliothèques partagées
# 		(A empeché des erreurs aussi, source stackoverflow)
	
install: test_buttons
	@echo envoi de test_buttons au RPI $(IPRPI)
	sshpass -v -p raspberry scp test_buttons pi@$(IPRPI):/home/pi/Desktop

clean: 
	rm -rf test_buttons