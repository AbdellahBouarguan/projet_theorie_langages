#include "../include/automate.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Automaton *create_automaton()
{
  Automaton *automate = (Automaton *)malloc(sizeof(Automaton));
  automate->num_etats = 0;
  automate->num_transitions = 0;
  automate->num_alphabet = 0;
  for (int i = 0; i < MAX_ETATS; i++)
  {
    automate->is_final[i] = false;
    automate->is_initial[i] = false;
  }
  return automate;
}

void free_automaton(Automaton *automate)
{
  if (automate)
  {
    free(automate);
  }
}

// Extraire l'identifiant d’un état à partir de chaînes comme "q1", "S2" ou simplement "1"
static int parse_etat_id(const char *str)
{
  while (*str && !isdigit(*str))
  {
    str++;
  }
  if (*str)
  {
    return atoi(str);
  }
  return -1;
}

static int get_or_create_etat_idx(Automaton *automate, int etat_id)
{
  for (int i = 0; i < automate->num_etats; i++)
  {
    if (automate->etats[i] == etat_id)
      return i; // Index trouvé
  }
  if (automate->num_etats < MAX_ETATS)
  {
    int idx = automate->num_etats++;
    automate->etats[idx] = etat_id;
    return idx;
  }
  return -1;
}

static void add_to_alphabet(Automaton *automate, const char *label)
{
  for (int i = 0; i < automate->num_alphabet; i++)
  {
    if (strcmp(automate->alphabet[i], label) == 0)
      return;
  }
  if (automate->num_alphabet < MAX_ALPHABET)
  {
    strcpy(automate->alphabet[automate->num_alphabet++], label);
  }
}

bool load_automaton_from_dot(Automaton *automate, const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (!file)
  {
    printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
    return false;
  }

  char line[256];

  while (fgets(line, sizeof(line), file))
  {
    char *arrow_pos = strstr(line, "->");

    // Nous nous intéressons uniquement aux lignes contenant des transitions
    if (arrow_pos)
    {
      char from_str[64] = {0};
      char to_str[64] = {0};
      char label_str[64] = "epsilon";

      // Solution pour les nœuds avec chaîne vide (ex: "" -> 0)
      // Remplacer "" par un identifiant temporaire "IN" afin que sscanf puisse l’analyser comme un seul token
      char temp_line[256];
      strcpy(temp_line, line);
      char *empty_node = strstr(temp_line, "\"\"");
      if (empty_node && empty_node < (temp_line + (arrow_pos - line)))
      {
        empty_node[0] = 'I';
        empty_node[1] = 'N'; // Transforme "" en IN
      }

      // Extraire la source et la destination
      if (sscanf(temp_line, " %s -> %s", from_str, to_str) != 2)
        continue;

      // Supprimer les crochets ou points-virgules à la fin de la destination
      char *bracket = strchr(to_str, '[');
      if (bracket)
        *bracket = '\0';
      char *semi = strchr(to_str, ';');
      if (semi)
        *semi = '\0';

      // 1. Détection de l’état initial ("IN" -> État)
      if (strcmp(from_str, "IN") == 0)
      {
        int to_id = parse_etat_id(to_str);
        if (to_id != -1)
        {
          int idx = get_or_create_etat_idx(automate, to_id);
          automate->is_initial[idx] = true;
        }
        continue;
      }

      // 2. Détection de l’état final (État -> "fin*")
      if (strncmp(to_str, "fin", 3) == 0)
      {
        int from_id = parse_etat_id(from_str);
        if (from_id != -1)
        {
          int idx = get_or_create_etat_idx(automate, from_id);
          automate->is_final[idx] = true;
        }
        continue;
      }

      // 3. Traitement des transitions normales
      int from_id = parse_etat_id(from_str);
      int to_id = parse_etat_id(to_str);

      if (from_id != -1 && to_id != -1)
      {
        char *label_pos = strstr(line, "label");
        if (label_pos)
        {
          char *quote1 = strchr(label_pos, '"');
          if (quote1)
          {
            char *quote2 = strchr(quote1 + 1, '"');
            if (quote2)
            {
              strncpy(label_str, quote1 + 1, quote2 - quote1 - 1);
              label_str[quote2 - quote1 - 1] = '\0';
            }
          }
          else
          {
            sscanf(label_pos, "label=%63[^ \t;,\]]", label_str);
          }
        }

        int from_idx = get_or_create_etat_idx(automate, from_id);
        int to_idx = get_or_create_etat_idx(automate, to_id);

        if (automate->num_transitions < MAX_TRANSITIONS)
        {
          Transition *t = &automate->transitions[automate->num_transitions++];
          t->from_etat = automate->etats[from_idx];
          t->to_etat = automate->etats[to_idx];
          strcpy(t->label, label_str);

          if (strcmp(label_str, "epsilon") != 0)
          {
            add_to_alphabet(automate, label_str);
          }
        }
      }
    }
  }
  // Par défaut, considérer le premier état (index 0) comme initial
  // si aucun état initial n’est explicitement défini via "" -> X
  bool has_initial = false;
  for (int i = 0; i < automate->num_etats; i++)
  {
    if (automate->is_initial[i])
    {
      has_initial = true;
      break;
    }
  }
  if (!has_initial && automate->num_etats > 0)
  {
    automate->is_initial[0] = true;
  }

  fclose(file);
  return true;
}