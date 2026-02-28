#include "../include/automate.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Automaton *create_automaton() {
  Automaton *automate = (Automaton *)malloc(sizeof(Automaton));
  automate->num_states = 0;
  automate->num_transitions = 0;
  automate->num_alphabet = 0;
  for (int i = 0; i < MAX_STATES; i++) {
    automate->is_final[i] = false;
    automate->is_initial[i] = false;
  }
  return automate;
}

void free_automaton(Automaton *automate) {
  if (automate) {
    free(automate);
  }
}

// Fonction utilitaire pour extraire le numéro de l'état (ex: S1 -> 1, q2 -> 2)
static int parse_state_id(const char *str) {
  while (*str && !isdigit(*str)) {
    str++;
  }
  if (*str) {
    return atoi(str);
  }
  return -1;
}

static int get_or_create_state_idx(Automaton *automate, int state_id) {
  for (int i = 0; i < automate->num_states; i++) {
    if (automate->states[i] == state_id)
      return i; // retourne l'index
  }
  if (automate->num_states < MAX_STATES) {
    int idx = automate->num_states++;
    automate->states[idx] = state_id;
    return idx;
  }
  return -1;
}

static void add_to_alphabet(Automaton *automate, const char *label) {
  for (int i = 0; i < automate->num_alphabet; i++) {
    if (strcmp(automate->alphabet[i], label) == 0)
      return;
  }
  if (automate->num_alphabet < MAX_ALPHABET) {
    strcpy(automate->alphabet[automate->num_alphabet++], label);
  }
}

bool load_automaton_from_dot(Automaton *automate, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
    return false;
  }

  char line[256];
  bool in_doublecircle = false;
  bool in_initial = false;

  while (fgets(line, sizeof(line), file)) {
    // Supprimer le point-virgule à la fin s'il y en a un
    char *semicolon = strchr(line, ';');
    if (semicolon)
      *semicolon = ' ';

    if (strstr(line, "shape=\"doublecircle\"") ||
        strstr(line, "shape=doublecircle") ||
        strstr(line, "shape = doublecircle")) {
      // Lecture des états finaux
      char *token = strtok(line, " \t\n{}[];,=");
      while (token != NULL) {
        if (strcmp(token, "node") != 0 && strcmp(token, "shape") != 0 &&
            strcmp(token, "doublecircle") != 0 &&
            strcmp(token, "circle") != 0 && strcmp(token, "point") != 0 &&
            strcmp(token, "rankdir") != 0 && strcmp(token, "LR") != 0) {
          int state_id = parse_state_id(token);
          if (state_id != -1) {
            int idx = get_or_create_state_idx(automate, state_id);
            if (idx != -1)
              automate->is_final[idx] = true;
          }
        }
        token = strtok(NULL, " \t\n{}[];,=");
      }
    } else if (strstr(line, "->")) {
      // Lecture des transitions
      char *arrow_pos = strstr(line, "->");
      char from_str[64] = {0};
      char to_str[64] = {0};
      char label_str[64] = {0};

      // Extraire noeud de départ
      sscanf(line, " %s ->", from_str);
      // Extraire noeud d'arrivée
      sscanf(arrow_pos + 2, " %s ", to_str);

      int from_id = parse_state_id(from_str);
      int to_id = parse_state_id(to_str);

      if (from_id == -1 &&
          strstr(from_str, "qi")) { // état initial dummy souvent appelé qi
        if (to_id != -1) {
          int idx = get_or_create_state_idx(automate, to_id);
          if (idx != -1)
            automate->is_initial[idx] = true;
        }
        continue;
      }

      char *label_pos = strstr(line, "label");
      if (label_pos) {
        char *quote1 = strchr(label_pos, '"');
        if (quote1) {
          char *quote2 = strchr(quote1 + 1, '"');
          if (quote2) {
            strncpy(label_str, quote1 + 1, quote2 - quote1 - 1);
          }
        } else {
          sscanf(label_pos, "label=%s", label_str);
        }
      } else {
        strcpy(label_str, "epsilon"); // transition epsilon si pas de label ?
      }

      if (from_id != -1 && to_id != -1 &&
          automate->num_transitions < MAX_TRANSITIONS) {
        int from_idx = get_or_create_state_idx(automate, from_id);
        int to_idx = get_or_create_state_idx(automate, to_id);

        if (from_idx != -1 && to_idx != -1) {
          Transition *t = &automate->transitions[automate->num_transitions++];
          t->from_state = automate->states[from_idx];
          t->to_state = automate->states[to_idx];
          strcpy(t->label, label_str);

          if (strcmp(label_str, "epsilon") != 0) {
            add_to_alphabet(automate, label_str);
          }
        }
      }
    }
  }

  // Assigner un état initial par défaut si aucun n'est trouvé explicitement
  // (node 0)
  bool has_initial = false;
  for (int i = 0; i < automate->num_states; i++) {
    if (automate->is_initial[i]) {
      has_initial = true;
      break;
    }
  }
  if (!has_initial && automate->num_states > 0) {
    automate->is_initial[0] = true; // par défaut le premier vu
  }

  fclose(file);
  return true;
}
