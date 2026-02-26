CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

# Détection du système d'exploitation pour l'extension de l'exécutable
ifeq ($(OS),Windows_NT)
    TARGET = bin/programme_automate.exe
else
    TARGET = bin/programme_automate
endif

# Récupération automatique de tous les fichiers .c dans src/
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)

all: $(TARGET)

# Règle pour créer l'exécutable final
$(TARGET): $(OBJ)
	@mkdir -p bin
	$(CC) $(OBJ) -o $(TARGET)

# Règle pour compiler chaque fichier .c en .o
obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour tout nettoyer
clean:
	rm -rf obj/*.o bin/programme_automate bin/programme_automate.exe

.PHONY: all clean