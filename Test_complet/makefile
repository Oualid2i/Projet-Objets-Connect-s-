# --- Chemins et configurations ---
PATH_CCC = /home/bob/Desktop/OC_OUAZZANI_LENGLET/devProjet/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc
PI_IP = 192.168.1.64
PI_USER = pi
PI_PASS = raspberry
PI_DIR = /home/pi/Desktop/TestModules/Test_complet

# --- Règles principales ---

# Installer le test complet
install-test-complet: build-api build-cross link-remote
	@echo "Test complet installé sur le Raspberry Pi dans $(PI_DIR)/test_complet"

# Installer les tests modulaires
install-tests-modulaires: test_7segments test_buttons test_buzzer test_matriceLEDS
	@echo "Tous les tests modulaires installés sur le Raspberry Pi"

# --- Construction du test complet ---

# 1. Compiler l'API sur le Pi (libcurl/libcjson non dispo en cross-compilation)
build-api:
	@echo "Transfert et compilation de l'API sur le Raspberry Pi..."
	sshpass -p $(PI_PASS) ssh $(PI_USER)@$(PI_IP) "mkdir -p $(PI_DIR)"
	sshpass -p $(PI_PASS) scp ../APIquestions/APIquestions.c ../APIquestions/APIquestions.h $(PI_USER)@$(PI_IP):$(PI_DIR)/
	sshpass -p $(PI_PASS) ssh $(PI_USER)@$(PI_IP) "cd $(PI_DIR) && gcc -c APIquestions.c -o APIquestions.o -lcurl -lcjson"

# 2. Cross-compiler localement
build-cross:
	@echo "Cross-compilation locale..."
	$(PATH_CCC) -c test_complet.c -o test_complet.o -I../7segments -I../buttons -I../Buzzer -I../MatriceLEDS -I../APIquestions -std=gnu99 -D'TARGET="PI"'
	$(PATH_CCC) -c ../7segments/biblio_7segments.c -o biblio_7segments.o -I../7segments -std=gnu99 -D'TARGET="PI"'
	$(PATH_CCC) -c ../buttons/biblio_buttons.c -o biblio_buttons.o -I../buttons -std=gnu99 -D'TARGET="PI"'
	$(PATH_CCC) -c ../Buzzer/biblio_buzzer.c -o biblio_buzzer.o -I../Buzzer -std=gnu99 -D'TARGET="PI"'
	$(PATH_CCC) -c ../MatriceLEDS/biblio_matriceLEDS.c -o biblio_matriceLEDS.o -I../MatriceLEDS -std=gnu99 -D'TARGET="PI"'

# 3. Linker sur le Pi
link-remote:
	@echo "Transfert des fichiers objets..."
	sshpass -p $(PI_PASS) ssh $(PI_USER)@$(PI_IP) "mkdir -p $(PI_DIR)"
	sshpass -p $(PI_PASS) scp test_complet.o biblio_7segments.o biblio_buttons.o biblio_buzzer.o biblio_matriceLEDS.o $(PI_USER)@$(PI_IP):$(PI_DIR)/
	@echo "Linkage sur le Raspberry Pi..."
	sshpass -p $(PI_PASS) ssh $(PI_USER)@$(PI_IP) "cd $(PI_DIR) && gcc test_complet.o biblio_7segments.o biblio_buttons.o biblio_buzzer.o biblio_matriceLEDS.o APIquestions.o -o test_complet -lwiringPi -lcurl -lcjson -lm"

# --- Tests modulaires ---

test_7segments:
	@echo "Cross-compilation de test_7segments..."
	cd ../7segments && $(PATH_CCC) -std=gnu99 -D'TARGET="PI"' biblio_7segments.c biblio_7segments_test.c -o test_7segments -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
	@echo "Transfert de test_7segments..."
	sshpass -p $(PI_PASS) ssh $(PI_USER)@$(PI_IP) "mkdir -p /home/pi/7segments"
	sshpass -p $(PI_PASS) scp ../7segments/test_7segments $(PI_USER)@$(PI_IP):/home/pi/7segments/
	@echo "test_7segments installé"

test_buttons:
	@echo "Cross-compilation de test_buttons..."
	cd ../buttons && $(PATH_CCC) -std=gnu99 -D'TARGET="PI"' biblio_buttons.c biblio_buttons_test.c -o test_buttons -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
	@echo "Transfert de test_buttons..."
	sshpass -p $(PI_PASS) ssh $(PI_USER)@$(PI_IP) "mkdir -p /home/pi/buttons"
	sshpass -p $(PI_PASS) scp ../buttons/test_buttons $(PI_USER)@$(PI_IP):/home/pi/buttons/
	@echo "test_buttons installé"

test_buzzer:
	@echo "Cross-compilation de test_buzzer..."
	cd ../Buzzer && $(PATH_CCC) -std=gnu99 -D'TARGET="PI"' biblio_buzzer.c biblio_buzzer_test.c -o test_buzzer -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
	@echo "Transfert de test_buzzer..."
	sshpass -p $(PI_PASS) ssh $(PI_USER)@$(PI_IP) "mkdir -p /home/pi/Buzzer"
	sshpass -p $(PI_PASS) scp ../Buzzer/test_buzzer $(PI_USER)@$(PI_IP):/home/pi/Buzzer/
	@echo "test_buzzer installé"

test_matriceLEDS:
	@echo "Cross-compilation de test_matriceLEDS..."
	cd ../MatriceLEDS && $(PATH_CCC) -std=gnu99 -D'TARGET="PI"' biblio_matriceLEDS.c biblio_matricesLEDS_test.c -o test_matriceLEDS -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
	@echo "Transfert de test_matriceLEDS..."
	sshpass -p $(PI_PASS) ssh $(PI_USER)@$(PI_IP) "mkdir -p /home/pi/MatriceLEDS"
	sshpass -p $(PI_PASS) scp ../MatriceLEDS/test_matriceLEDS $(PI_USER)@$(PI_IP):/home/pi/MatriceLEDS/
	@echo "test_matriceLEDS installé"

# --- Nettoyage ---
clean:
	rm -f *.o test_7segments test_buttons test_buzzer test_matriceLEDS
	cd ../7segments && rm -f test_7segments *.o
	cd ../buttons && rm -f test_buttons *.o
	cd ../Buzzer && rm -f test_buzzer *.o
	cd ../MatriceLEDS && rm -f test_matriceLEDS *.o

