# Version minimale requise de CMake
cmake_minimum_required(VERSION 3.10)

# Nom du projet et langage (C)
project(ProjetTheorieLangages C)

# Options de compilation (équivalent de tes CFLAGS)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Indiquer où se trouvent les fichiers .h
include_directories(include)

# Récupérer automatiquement tous les fichiers .c dans le dossier src/
file(GLOB SOURCES "src/*.c")

# Créer l'exécutable final
add_executable(programme_automate ${SOURCES})