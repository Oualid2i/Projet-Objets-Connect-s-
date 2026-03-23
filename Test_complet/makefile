ifneq ($(MAKECMDGOALS),clean)
ifndef PATH_CCC
$(error PATH_CCC is not set. Run 'source ../env.sh' or pass PATH_CCC=/path/to/arm-linux-gnueabihf-gcc)
endif
endif

PI_USER ?= pi
PI_IP ?=
PI_PASS ?= raspberry
PI_HOST ?= $(if $(PI_IP),$(PI_USER)@$(PI_IP),)
PI_DIR ?= /home/pi/Desktop/TestModules/Test_complet
SSH_OPTS ?=
SCP_OPTS ?=
SSH ?= ssh
SCP ?= scp
SSHPASS_BIN := $(shell command -v sshpass 2>/dev/null)

REMOTE_TARGETS = build-api link-remote install-test-complet run-remote \
	install-tests-modulaires test_7segments test_buttons test_buzzer test_matriceLEDS prepare-remote

ifneq ($(filter $(MAKECMDGOALS),$(REMOTE_TARGETS)),)
ifeq ($(strip $(PI_HOST)),)
$(error PI_IP is not set. Example: make PI_IP=192.168.1.42 install-test-complet)
endif
endif

ifneq ($(strip $(PI_PASS)),)
ifneq ($(strip $(SSHPASS_BIN)),)
SSH_CMD = sshpass -p "$(PI_PASS)" $(SSH) $(SSH_OPTS) $(PI_HOST)
SCP_CMD = sshpass -p "$(PI_PASS)" $(SCP) $(SCP_OPTS)
else
SSH_CMD = $(SSH) $(SSH_OPTS) $(PI_HOST)
SCP_CMD = $(SCP) $(SCP_OPTS)
endif
else
SSH_CMD = $(SSH) $(SSH_OPTS) $(PI_HOST)
SCP_CMD = $(SCP) $(SCP_OPTS)
endif

# --- Règles principales ---

# Installer le test complet
install-test-complet: build-api build-cross link-remote
	@echo "Test complet installé sur le Raspberry Pi dans $(PI_DIR)/test_complet"

# Installer les tests modulaires
install-tests-modulaires: test_7segments test_buttons test_buzzer test_matriceLEDS
	@echo "Tous les tests modulaires installés sur le Raspberry Pi"

# Preparer une nouvelle valisette (dependances systeme pour le build distant)
prepare-remote:
	@echo "Installation des dependances systeme sur $(PI_HOST)..."
	$(SSH_CMD) "sudo apt-get update && sudo apt-get install -y build-essential libcurl4-openssl-dev libcjson-dev"

# --- Construction du test complet ---

# 1. Compiler l'API sur le Pi (libcurl/libcjson non dispo en cross-compilation)
build-api:
	@echo "Transfert et compilation de l'API sur le Raspberry Pi..."
	$(SSH_CMD) "mkdir -p $(PI_DIR)"
	$(SCP_CMD) ../APIquestions/APIquestions.c ../APIquestions/APIquestions.h $(PI_HOST):$(PI_DIR)/
	$(SSH_CMD) "cd $(PI_DIR) && gcc -c APIquestions.c -o APIquestions.o -lcurl -lcjson"

# 2. Cross-compiler localement
build-cross:
	@echo "Cross-compilation locale..."
	$(PATH_CCC) -c test_complet.c -o test_complet.o -I../7segments -I../buttons -I../Buzzer -I../MatriceLEDS -I../APIquestions -std=gnu99 -D'TARGET="PI"'
	$(PATH_CCC) -c game_runtime.c -o game_runtime.o -I../7segments -I../buttons -I../Buzzer -I../MatriceLEDS -std=gnu99 -D'TARGET="PI"'
	$(PATH_CCC) -c ../7segments/biblio_7segments.c -o biblio_7segments.o -I../7segments -std=gnu99 -D'TARGET="PI"'
	$(PATH_CCC) -c ../buttons/biblio_buttons.c -o biblio_buttons.o -I../buttons -std=gnu99 -D'TARGET="PI"'
	$(PATH_CCC) -c ../Buzzer/biblio_buzzer.c -o biblio_buzzer.o -I../Buzzer -std=gnu99 -D'TARGET="PI"'
	$(PATH_CCC) -c ../MatriceLEDS/biblio_matriceLEDS.c -o biblio_matriceLEDS.o -I../MatriceLEDS -std=gnu99 -D'TARGET="PI"'

# 3. Linker sur le Pi
link-remote:
	@echo "Transfert des fichiers objets..."
	$(SSH_CMD) "mkdir -p $(PI_DIR)"
	$(SCP_CMD) test_complet.o game_runtime.o biblio_7segments.o biblio_buttons.o biblio_buzzer.o biblio_matriceLEDS.o ../7segments/libwiringPi.so $(PI_HOST):$(PI_DIR)/
	@echo "Installation de libwiringPi.so sur le Raspberry Pi..."
	$(SSH_CMD) "cd $(PI_DIR) && sudo cp -f libwiringPi.so /usr/local/lib/libwiringPi.so && sudo chmod 755 /usr/local/lib/libwiringPi.so && sudo ldconfig"
	@echo "Linkage sur le Raspberry Pi..."
	$(SSH_CMD) "cd $(PI_DIR) && gcc test_complet.o game_runtime.o biblio_7segments.o biblio_buttons.o biblio_buzzer.o biblio_matriceLEDS.o APIquestions.o -o test_complet -L/usr/local/lib -L. -Wl,-rpath,\$$ORIGIN -lwiringPi -lcurl -lcjson -lm -lrt"

run-remote:
	@echo "Execution du test complet sur $(PI_HOST)..."
	$(SSH_CMD) "cd $(PI_DIR) && sudo ./test_complet"

# --- Tests modulaires ---

test_7segments:
	@echo "Cross-compilation de test_7segments..."
	cd ../7segments && $(PATH_CCC) -std=gnu99 -D'TARGET="PI"' biblio_7segments.c biblio_7segments_test.c -o test_7segments -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
	@echo "Transfert de test_7segments..."
	$(SSH_CMD) "mkdir -p /home/pi/7segments"
	$(SCP_CMD) ../7segments/test_7segments $(PI_HOST):/home/pi/7segments/
	@echo "test_7segments installé"

test_buttons:
	@echo "Cross-compilation de test_buttons..."
	cd ../buttons && $(PATH_CCC) -std=gnu99 -D'TARGET="PI"' biblio_buttons.c biblio_buttons_test.c -o test_buttons -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
	@echo "Transfert de test_buttons..."
	$(SSH_CMD) "mkdir -p /home/pi/buttons"
	$(SCP_CMD) ../buttons/test_buttons $(PI_HOST):/home/pi/buttons/
	@echo "test_buttons installé"

test_buzzer:
	@echo "Cross-compilation de test_buzzer..."
	cd ../Buzzer && $(PATH_CCC) -std=gnu99 -D'TARGET="PI"' biblio_buzzer.c biblio_buzzer_test.c -o test_buzzer -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
	@echo "Transfert de test_buzzer..."
	$(SSH_CMD) "mkdir -p /home/pi/Buzzer"
	$(SCP_CMD) ../Buzzer/test_buzzer $(PI_HOST):/home/pi/Buzzer/
	@echo "test_buzzer installé"

test_matriceLEDS:
	@echo "Cross-compilation de test_matriceLEDS..."
	cd ../MatriceLEDS && $(PATH_CCC) -std=gnu99 -D'TARGET="PI"' biblio_matriceLEDS.c biblio_matricesLEDS_test.c -o test_matriceLEDS -I. -L. -lwiringPi -Wl,--allow-shlib-undefined
	@echo "Transfert de test_matriceLEDS..."
	$(SSH_CMD) "mkdir -p /home/pi/MatriceLEDS"
	$(SCP_CMD) ../MatriceLEDS/test_matriceLEDS $(PI_HOST):/home/pi/MatriceLEDS/
	@echo "test_matriceLEDS installé"

# --- Nettoyage ---
clean:
	rm -f *.o test_7segments test_buttons test_buzzer test_matriceLEDS test_complet
	cd ../7segments && rm -f test_7segments *.o
	cd ../buttons && rm -f test_buttons *.o
	cd ../Buzzer && rm -f test_buzzer *.o
	cd ../MatriceLEDS && rm -f test_matriceLEDS *.o
