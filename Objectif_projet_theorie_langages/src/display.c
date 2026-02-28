#include "../include/automate.h"
#include <stdio.h>

void display_automaton(const Automaton *automate) {
  if (!automate) {
    printf("L'automate est vide ou n'a pas ete charge.\n");
    return;
  }

  printf("\n=== Informations de l'Automate ===\n");

  // Affichage des états
  printf("Etats (Q) : ");
  if (automate->num_states == 0) {
    printf("Aucun etat défini.\n");
  } else {
    printf("{ ");
    for (int i = 0; i < automate->num_states; i++) {
      printf("q%d%s", automate->states[i],
             (i == automate->num_states - 1) ? "" : ", ");
    }
    printf(" }\n");
  }

  // Affichage de l'alphabet
  printf("Alphabet (Sigma) : ");
  if (automate->num_alphabet == 0) {
    printf("Aucun alphabet défini.\n");
  } else {
    printf("{ ");
    for (int i = 0; i < automate->num_alphabet; i++) {
      printf("%s%s", automate->alphabet[i],
             (i == automate->num_alphabet - 1) ? "" : ", ");
    }
    printf(" }\n");
  }

  // Affichage des états initiaux (I)
  printf("Etats initiaux (I) : ");
  bool first_initial = true;
  printf("{ ");
  for (int i = 0; i < automate->num_states; i++) {
    if (automate->is_initial[i]) {
      if (!first_initial)
        printf(", ");
      printf("q%d", automate->states[i]);
      first_initial = false;
    }
  }
  printf(" }\n");

  // Affichage des états finaux (F)
  printf("Etats finaux (F) : ");
  bool first_final = true;
  printf("{ ");
  for (int i = 0; i < automate->num_states; i++) {
    if (automate->is_final[i]) {
      if (!first_final)
        printf(", ");
      printf("q%d", automate->states[i]);
      first_final = false;
    }
  }
  printf(" }\n");

  // Affichage des transitions (delta)
  printf("Transitions (delta) :\n");
  if (automate->num_transitions == 0) {
    printf("  Aucune transition.\n");
  } else {
    for (int i = 0; i < automate->num_transitions; i++) {
      Transition t = automate->transitions[i];
      printf("  q%d --(%s)--> q%d\n", t.from_state, t.label, t.to_state);
    }
  }
  printf("==================================\n\n");
}
