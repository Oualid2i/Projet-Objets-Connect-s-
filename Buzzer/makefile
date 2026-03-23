ifneq ($(MAKECMDGOALS),clean)
ifndef PATH_CCC
$(error PATH_CCC is not set. Run 'source ../env.sh' or pass PATH_CCC=/path/to/arm-linux-gnueabihf-gcc)
endif
endif

IPRPI ?= 192.168.176.213

all: test_buzzer

test_buzzer: biblio_buzzer.c biblio_buzzer_test.c
	$(PATH_CCC) -std=gnu99 -D'TARGET="PI"' $^ -o $@ -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
# -std=gnu99                : Force le standard C99 avec extensions GNU (à gargder car Linaro est ancien) 
# 		(j'ai eu des problèmes de compilation et cette solution a marché)
# -I.                       : Cherche les fichiers d'en-tête dans le dossier courant
# -L.                       : Cherche les bibliothèques dans le dossier courant
# -lwiringPi                : Lie la librairie WiringPi
# -Wl,--allow-shlib-undefined : Demande au linker d'ignorer les symboles non trouvés dans les bibliothèques partagées
# 		(A empeché des erreurs aussi, source stackoverflow)
	
install: test_buzzer
	@echo envoi de test_buzzer au RPI $(IPRPI)
	sshpass -v -p raspberry scp test_buzzer pi@$(IPRPI):/home/pi/Desktop

clean: 
	rm -rf test_buzzer