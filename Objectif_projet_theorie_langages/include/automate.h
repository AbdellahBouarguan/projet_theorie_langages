#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <stdbool.h>

#define MAX_STATES 100
#define MAX_TRANSITIONS 500
#define MAX_ALPHABET 50
#define MAX_LABEL_LEN 64

// Structure pour une transition
typedef struct {
    int from_state;
    int to_state;
    char label[MAX_LABEL_LEN];
} Transition;

// Structure pour l'automate
typedef struct {
    int num_states;
    int states[MAX_STATES];          // Liste des identifiants (ex: 0, 1, 2)
    bool is_final[MAX_STATES];       // Indique si l'état est final
    bool is_initial[MAX_STATES];     // Indique si l'état est initial

    int num_transitions;
    Transition transitions[MAX_TRANSITIONS];

    int num_alphabet;
    char alphabet[MAX_ALPHABET][MAX_LABEL_LEN];
} Automaton;

// Fonctionnalités principales (Phase 1)
Automaton* create_automaton();
void free_automaton(Automaton* automate);
bool load_automaton_from_dot(Automaton* automate, const char* filename);
void display_automaton(const Automaton* automate);

#endif // AUTOMATE_H
