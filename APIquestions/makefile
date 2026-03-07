CC = gcc
# On ajoute les flags pour curl et cjson
CFLAGS = -Wall -Wextra -fPIC $(shell curl-config --cflags)
# LDFLAGS inclut curl et cjson
LDFLAGS = -lcurl -lcjson

LIB_NAME = libapiquestions.a
OBJ = APIquestions.o

all: $(LIB_NAME) testquestions

APIquestions.o: APIquestions.c APIquestions.h
	$(CC) $(CFLAGS) -c APIquestions.c -o APIquestions.o

# Création de la lib statique
$(LIB_NAME): $(OBJ)
	ar rcs $(LIB_NAME) $(OBJ)
	@echo "Library built. Don't forget to link with -lcurl -lcjson when using it."

testquestions: main.c $(LIB_NAME)
	$(CC) main.c -L. -lapiquestions $(LDFLAGS) -o testquestions

clean:
	rm -f *.o $(LIB_NAME) testquestions