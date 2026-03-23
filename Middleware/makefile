ifneq ($(MAKECMDGOALS),clean)
ifndef PATH_CCC
$(error PATH_CCC is not set. Run 'source ../env.sh' or pass PATH_CCC=/path/to/arm-linux-gnueabihf-gcc)
endif
endif

NATIVE_CC ?= gcc

HOST_BIN = ../sa_main
CLIENT_BIN = ../client_valisette

HOST_CFLAGS = -Wall -Wextra -std=gnu99 -I. -I../sa_partie -I../game_engine -I../APIquestions
CLIENT_CFLAGS = -Wall -Wextra -std=gnu99 -I. -I../Test_complet -I../7segments -I../buttons -I../Buzzer -I../MatriceLEDS -D'TARGET="PI"'

HOST_OBJS = ../sa_partie/sa_main.o ../sa_partie/sa_partie.o ../game_engine/game_engine.o middleware_protocol_host.o ../APIquestions/APIquestions.o
CLIENT_OBJS = client_main.o middleware_client.o middleware_protocol_client.o ../Test_complet/game_runtime.o client_7segments.o client_buttons.o client_buzzer.o client_matrice.o

HOST_LIBS = -lcurl -lcjson
CLIENT_LIBS = -lrt -L../buttons -lwiringPi -Wl,--allow-shlib-undefined

all: $(HOST_BIN) $(CLIENT_BIN)

$(HOST_BIN): $(HOST_OBJS)
	$(NATIVE_CC) $(HOST_OBJS) -o $(HOST_BIN) $(HOST_LIBS)

$(CLIENT_BIN): $(CLIENT_OBJS)
	$(PATH_CCC) $(CLIENT_OBJS) -o $(CLIENT_BIN) $(CLIENT_LIBS)

../sa_partie/sa_main.o: ../sa_partie/sa_main.c ../sa_partie/sa_partie.h
	$(NATIVE_CC) $(HOST_CFLAGS) -c ../sa_partie/sa_main.c -o ../sa_partie/sa_main.o

../sa_partie/sa_partie.o: ../sa_partie/sa_partie.c ../sa_partie/sa_partie.h ../game_engine/game_engine.h middleware_protocol.h
	$(NATIVE_CC) $(HOST_CFLAGS) -c ../sa_partie/sa_partie.c -o ../sa_partie/sa_partie.o

../game_engine/game_engine.o: ../game_engine/game_engine.c ../game_engine/game_engine.h middleware_protocol.h ../APIquestions/APIquestions.h
	$(NATIVE_CC) $(HOST_CFLAGS) -c ../game_engine/game_engine.c -o ../game_engine/game_engine.o

../APIquestions/APIquestions.o: ../APIquestions/APIquestions.c ../APIquestions/APIquestions.h
	$(NATIVE_CC) -Wall -Wextra -std=gnu99 $(shell curl-config --cflags) -c ../APIquestions/APIquestions.c -o ../APIquestions/APIquestions.o

middleware_protocol_host.o: middleware_protocol.c middleware_protocol.h
	$(NATIVE_CC) $(HOST_CFLAGS) -c middleware_protocol.c -o $@

client_main.o: client_main.c middleware_client.h
	$(PATH_CCC) $(CLIENT_CFLAGS) -c client_main.c -o $@

middleware_client.o: middleware_client.c middleware_client.h middleware_protocol.h ../Test_complet/game_runtime.h
	$(PATH_CCC) $(CLIENT_CFLAGS) -c middleware_client.c -o $@

middleware_protocol_client.o: middleware_protocol.c middleware_protocol.h
	$(PATH_CCC) $(CLIENT_CFLAGS) -c middleware_protocol.c -o $@

../Test_complet/game_runtime.o: ../Test_complet/game_runtime.c ../Test_complet/game_runtime.h
	$(PATH_CCC) $(CLIENT_CFLAGS) -c ../Test_complet/game_runtime.c -o ../Test_complet/game_runtime.o

client_7segments.o: ../7segments/biblio_7segments.c ../7segments/biblio_7segments.h
	$(PATH_CCC) $(CLIENT_CFLAGS) -c ../7segments/biblio_7segments.c -o $@

client_buttons.o: ../buttons/biblio_buttons.c ../buttons/biblio_buttons.h
	$(PATH_CCC) $(CLIENT_CFLAGS) -c ../buttons/biblio_buttons.c -o $@

client_buzzer.o: ../Buzzer/biblio_buzzer.c ../Buzzer/biblio_buzzer.h
	$(PATH_CCC) $(CLIENT_CFLAGS) -c ../Buzzer/biblio_buzzer.c -o $@

client_matrice.o: ../MatriceLEDS/biblio_matriceLEDS.c ../MatriceLEDS/biblio_matriceLEDS.h
	$(PATH_CCC) $(CLIENT_CFLAGS) -c ../MatriceLEDS/biblio_matriceLEDS.c -o $@

clean:
	rm -f *.o $(HOST_BIN) $(CLIENT_BIN)
	rm -f ../sa_partie/*.o ../game_engine/*.o ../APIquestions/APIquestions.o ../Test_complet/game_runtime.o
