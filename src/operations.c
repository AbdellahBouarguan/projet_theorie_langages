#include "../include/automate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction utilitaire interne copy_and_offset_states
bool copy_and_offset_states(Automaton *dest, const Automaton *src, int offset) {
  if (dest->num_etats + src->num_etats > MAX_ETATS) {
    printf("Erreur : Dépassement de la limite d'états (MAX_ETATS).\n");
    return false;
  }
  if (dest->num_transitions + src->num_transitions > MAX_TRANSITIONS) {
    printf("Erreur : Dépassement de la limite de transitions "
           "(MAX_TRANSITIONS).\n");
    return false;
  }

  // Copier les états et leurs propriétés
  for (int i = 0; i < src->num_etats; i++) {
    dest->etats[dest->num_etats] = src->etats[i] + offset;
    dest->is_initial[dest->num_etats] = src->is_initial[i];
    dest->is_final[dest->num_etats] = src->is_final[i];
    dest->num_etats++;
  }

  // Copier les transitions
  for (int i = 0; i < src->num_transitions; i++) {
    Transition *t_dest = &dest->transitions[dest->num_transitions];
    const Transition *t_src = &src->transitions[i];
    t_dest->from_etat = t_src->from_etat + offset;
    t_dest->to_etat = t_src->to_etat + offset;
    strcpy(t_dest->label, t_src->label);
    dest->num_transitions++;
  }

  // Copier l'alphabet (fusion sans doublons)
  for (int i = 0; i < src->num_alphabet; i++) {
    bool exists = false;
    for (int j = 0; j < dest->num_alphabet; j++) {
      if (strcmp(dest->alphabet[j], src->alphabet[i]) == 0) {
        exists = true;
        break;
      }
    }
    if (!exists && dest->num_alphabet < MAX_ALPHABET) {
      strcpy(dest->alphabet[dest->num_alphabet++], src->alphabet[i]);
    }
  }
  return true;
}

Automaton *concatener_automates(const Automaton *a1, const Automaton *a2) {
  Automaton *res = create_automaton();

  if (!copy_and_offset_states(res, a1, 0))
    return res;

  int max_id_a1 = 0;
  for (int i = 0; i < a1->num_etats; i++) {
    if (a1->etats[i] > max_id_a1) {
      max_id_a1 = a1->etats[i];
    }
  }
  int offset = max_id_a1 + 1;

  if (!copy_and_offset_states(res, a2, offset))
    return res;

  // Rendre les états finaux de a1 non-finaux et ajouter les epsilons vers les
  // initiaux de a2
  for (int i = 0; i < a1->num_etats; i++) {
    if (a1->is_final[i]) {
      res->is_final[i] = false;
      for (int j = 0; j < a2->num_etats; j++) {
        if (a2->is_initial[j]) {
          if (res->num_transitions >= MAX_TRANSITIONS) {
            printf("Erreur : Dépassement de la limite de transitions pour "
                   "epsilon.\n");
            return res;
          }

          int from_id = res->etats[i];
          int to_id = res->etats[a1->num_etats + j];

          Transition *t = &res->transitions[res->num_transitions++];
          t->from_etat = from_id;
          t->to_etat = to_id;
          strcpy(t->label, "epsilon"); // Etiquette epsilon
        }
      }
    }
  }

  // Les etats initiaux de a2 ne sont plus initiaux dans l'automate resultant
  for (int j = 0; j < a2->num_etats; j++) {
    res->is_initial[a1->num_etats + j] = false;
  }

  return res;
}

Automaton *union_automates(const Automaton *a1, const Automaton *a2) {
  Automaton *res = create_automaton();

  if (res->num_etats >= MAX_ETATS)
    return res;

  res->etats[0] = 0;
  res->is_initial[0] = true;
  res->is_final[0] = false;
  res->num_etats = 1;

  int offset1 = 1;
  if (!copy_and_offset_states(res, a1, offset1))
    return res;

  int max_id = 0;
  for (int i = 0; i < res->num_etats; i++) {
    if (res->etats[i] > max_id)
      max_id = res->etats[i];
  }
  int offset2 = max_id + 1;

  if (!copy_and_offset_states(res, a2, offset2))
    return res;

  for (int i = 1; i <= a1->num_etats; i++) {
    if (a1->is_initial[i - 1]) {
      res->is_initial[i] = false;
      if (res->num_transitions >= MAX_TRANSITIONS) {
        printf("Erreur : Dépassement de la limite de transitions.\n");
        return res;
      }
      Transition *t = &res->transitions[res->num_transitions++];
      t->from_etat = 0;
      t->to_etat = res->etats[i];
      strcpy(t->label, "epsilon");
    }
  }
  for (int i = a1->num_etats + 1; i < res->num_etats; i++) {
    if (a2->is_initial[i - a1->num_etats - 1]) {
      res->is_initial[i] = false;
      if (res->num_transitions >= MAX_TRANSITIONS) {
        printf("Erreur : Dépassement de la limite de transitions.\n");
        return res;
      }
      Transition *t = &res->transitions[res->num_transitions++];
      t->from_etat = 0;
      t->to_etat = res->etats[i];
      strcpy(t->label, "epsilon");
    }
  }

  // Nouvel état final commun
  if (res->num_etats >= MAX_ETATS) {
    printf("Erreur : Impossible d'ajouter l'état final, limite atteinte.\n");
    return res;
  }

  int idx_final = res->num_etats;
  int max_val = 0;
  for (int i = 0; i < res->num_etats; i++) {
    if (res->etats[i] > max_val)
      max_val = res->etats[i];
  }
  res->etats[idx_final] = max_val + 1;
  res->is_initial[idx_final] = false;
  res->is_final[idx_final] = true;
  res->num_etats++;

  // Lier anciens finaux au nouvel etat final via epsilon
  for (int i = 1; i < idx_final; i++) {
    if (res->is_final[i]) {
      res->is_final[i] = false;
      if (res->num_transitions >= MAX_TRANSITIONS) {
        printf("Erreur : Impossible d'ajouter les transitions finales.\n");
        return res;
      }
      Transition *t = &res->transitions[res->num_transitions++];
      t->from_etat = res->etats[i];
      t->to_etat = res->etats[idx_final];
      strcpy(t->label, "epsilon");
    }
  }

  return res;
}

Automaton *regex_to_nfa(const char *regex) {
  (void)regex;
  printf("Fonction regex_to_nfa non implémentée.\n");
  return create_automaton();
}

void supprimer_epsilon_transitions(Automaton *a) {
  (void)a;
  printf("Fonction supprimer_epsilon_transitions non implémentée.\n");
}
