#include "../include/automate.h"
#include <stdio.h>
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

static Automaton *etoile_automate(const Automaton *a) {
  Automaton *res = create_automaton();
  if (res->num_etats >= MAX_ETATS)
    return res;

  res->etats[0] = 0;
  res->is_initial[0] = true;
  res->is_final[0] = false;
  res->num_etats = 1;

  int offset = 1;
  if (!copy_and_offset_states(res, a, offset))
    return res;

  int max_id = 0;
  for (int i = 0; i < res->num_etats; i++) {
    if (res->etats[i] > max_id)
      max_id = res->etats[i];
  }

  if (res->num_etats >= MAX_ETATS)
    return res;
  int idx_final = res->num_etats;
  res->etats[idx_final] = max_id + 1;
  res->is_initial[idx_final] = false;
  res->is_final[idx_final] = true;
  res->num_etats++;

  int old_initials[MAX_ETATS], num_old_initials = 0;
  int old_finals[MAX_ETATS], num_old_finals = 0;

  for (int i = 1; i < idx_final; i++) {
    if (res->is_initial[i]) {
      old_initials[num_old_initials++] = i;
      res->is_initial[i] = false;
    }
    if (res->is_final[i]) {
      old_finals[num_old_finals++] = i;
      res->is_final[i] = false;
    }
  }

  if (res->num_transitions >= MAX_TRANSITIONS)
    return res;
  Transition *t = &res->transitions[res->num_transitions++];
  t->from_etat = res->etats[0];
  t->to_etat = res->etats[idx_final];
  strcpy(t->label, "epsilon");

  for (int k = 0; k < num_old_initials; k++) {
    if (res->num_transitions >= MAX_TRANSITIONS)
      return res;
    t = &res->transitions[res->num_transitions++];
    t->from_etat = res->etats[0];
    t->to_etat = res->etats[old_initials[k]];
    strcpy(t->label, "epsilon");
  }

  for (int f = 0; f < num_old_finals; f++) {
    if (res->num_transitions >= MAX_TRANSITIONS)
      return res;
    t = &res->transitions[res->num_transitions++];
    t->from_etat = res->etats[old_finals[f]];
    t->to_etat = res->etats[idx_final];
    strcpy(t->label, "epsilon");

    for (int k = 0; k < num_old_initials; k++) {
      if (res->num_transitions >= MAX_TRANSITIONS)
        return res;
      t = &res->transitions[res->num_transitions++];
      t->from_etat = res->etats[old_finals[f]];
      t->to_etat = res->etats[old_initials[k]];
      strcpy(t->label, "epsilon");
    }
  }

  return res;
}

static Automaton *create_char_automaton(char c) {
  Automaton *res = create_automaton();
  res->etats[0] = 0;
  res->is_initial[0] = true;
  res->is_final[0] = false;
  res->etats[1] = 1;
  res->is_initial[1] = false;
  res->is_final[1] = true;
  res->num_etats = 2;
  res->transitions[0].from_etat = 0;
  res->transitions[0].to_etat = 1;
  res->transitions[0].label[0] = c;
  res->transitions[0].label[1] = '\0';
  res->num_transitions = 1;
  res->num_alphabet = 1;
  res->alphabet[0][0] = c;
  res->alphabet[0][1] = '\0';
  return res;
}

static int precedence(char c) {
  if (c == '*')
    return 3;
  if (c == '.')
    return 2;
  if (c == '|')
    return 1;
  return 0;
}

static void apply_op(Automaton **stack_nfa, int *top_nfa, char *stack_op,
                     int *top_op) {
  if (*top_op < 0)
    return;
  char op = stack_op[(*top_op)--];

  if (op == '*') {
    if (*top_nfa < 0)
      return;
    Automaton *a = stack_nfa[*top_nfa];
    stack_nfa[*top_nfa] = etoile_automate(a);
    free_automaton(a);
  } else if (op == '.' || op == '|') {
    if (*top_nfa < 1)
      return;
    Automaton *a2 = stack_nfa[(*top_nfa)--];
    Automaton *a1 = stack_nfa[*top_nfa];

    if (op == '.')
      stack_nfa[*top_nfa] = concatener_automates(a1, a2);
    else
      stack_nfa[*top_nfa] = union_automates(a1, a2);

    free_automaton(a1);
    free_automaton(a2);
  }
}

Automaton *regex_to_nfa(const char *regex) {
  Automaton *stack_nfa[256];
  int top_nfa = -1;
  char stack_op[256];
  int top_op = -1;

  for (int i = 0; regex[i] != '\0'; i++) {
    char c = regex[i];
    if (c == '(') {
      stack_op[++top_op] = c;
    } else if (c == ')') {
      while (top_op >= 0 && stack_op[top_op] != '(') {
        apply_op(stack_nfa, &top_nfa, stack_op, &top_op);
      }
      if (top_op >= 0)
        top_op--; // Dépiler '('
    } else if (c == '*' || c == '.' || c == '|') {
      while (top_op >= 0 && stack_op[top_op] != '(' &&
             precedence(stack_op[top_op]) >= precedence(c)) {
        apply_op(stack_nfa, &top_nfa, stack_op, &top_op);
      }
      stack_op[++top_op] = c;
    } else {
      stack_nfa[++top_nfa] = create_char_automaton(c);
    }
  }

  while (top_op >= 0) {
    apply_op(stack_nfa, &top_nfa, stack_op, &top_op);
  }

  if (top_nfa >= 0) {
    return stack_nfa[0];
  }

  return create_automaton();
}

static int get_state_index(const Automaton *a, int id) {
  for (int i = 0; i < a->num_etats; i++) {
    if (a->etats[i] == id)
      return i;
  }
  return -1;
}

void supprimer_epsilon_transitions(Automaton *a) {
  bool closure[MAX_ETATS][MAX_ETATS];
  memset(closure, 0, sizeof(closure));

  for (int i = 0; i < a->num_etats; i++) {
    closure[i][i] = true;
  }

  for (int t = 0; t < a->num_transitions; t++) {
    if (strcmp(a->transitions[t].label, "epsilon") == 0) {
      int u = get_state_index(a, a->transitions[t].from_etat);
      int v = get_state_index(a, a->transitions[t].to_etat);
      if (u != -1 && v != -1) {
        closure[u][v] = true;
      }
    }
  }

  for (int k = 0; k < a->num_etats; k++) {
    for (int i = 0; i < a->num_etats; i++) {
      for (int j = 0; j < a->num_etats; j++) {
        if (closure[i][k] && closure[k][j]) {
          closure[i][j] = true;
        }
      }
    }
  }

  for (int i = 0; i < a->num_etats; i++) {
    for (int j = 0; j < a->num_etats; j++) {
      if (closure[i][j] && a->is_final[j]) {
        a->is_final[i] = true;
      }
    }
  }

  Transition new_transitions[MAX_TRANSITIONS];
  int new_count = 0;

  for (int i = 0; i < a->num_etats; i++) {
    for (int j = 0; j < a->num_etats; j++) {
      if (closure[i][j]) {
        for (int t = 0; t < a->num_transitions; t++) {
          if (strcmp(a->transitions[t].label, "epsilon") != 0 &&
              a->transitions[t].from_etat == a->etats[j]) {

            int k = get_state_index(a, a->transitions[t].to_etat);
            if (k != -1) {
              for (int l = 0; l < a->num_etats; l++) {
                if (closure[k][l]) {
                  bool exists = false;
                  for (int n = 0; n < new_count; n++) {
                    if (new_transitions[n].from_etat == a->etats[i] &&
                        new_transitions[n].to_etat == a->etats[l] &&
                        strcmp(new_transitions[n].label,
                               a->transitions[t].label) == 0) {
                      exists = true;
                      break;
                    }
                  }
                  if (!exists) {
                    if (new_count < MAX_TRANSITIONS) {
                      new_transitions[new_count].from_etat = a->etats[i];
                      new_transitions[new_count].to_etat = a->etats[l];
                      strcpy(new_transitions[new_count].label,
                             a->transitions[t].label);
                      new_count++;
                    } else {
                      printf("Erreur : Limite de MAX_TRANSITIONS atteinte.\n");
                      return;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  a->num_transitions = new_count;
  for (int i = 0; i < new_count; i++) {
    a->transitions[i] = new_transitions[i];
  }
}
