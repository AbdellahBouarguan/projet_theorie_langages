cmake_minimum_required(VERSION 3.10)
project(projet_theorie_langages)

set(CMAKE_C_STANDARD 99)

# Ajouter les warnings selon le compilateur
if (MSVC)
    # Pour Visual Studio / MSVC
    add_compile_options(/W4)
else()
    # Pour GCC / Clang
    add_compile_options(-Wall -Wextra)
endif()

include_directories(include)

file(GLOB SOURCES "src/*.c")
add_executable(programme_automate ${SOURCES})

set_target_properties(programme_automate PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)