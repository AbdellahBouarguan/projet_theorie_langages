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

static void get_epsilon_closure(const Automaton *a, int state_idx,
                                bool closure[MAX_ETATS]) {
  memset(closure, 0, MAX_ETATS * sizeof(bool));
  int stack[MAX_ETATS];
  int top = -1;

  closure[state_idx] = true;
  stack[++top] = state_idx;

  while (top >= 0) {
    int u = stack[top--];
    for (int i = 0; i < a->num_transitions; i++) {
      if (strcmp(a->transitions[i].label, "epsilon") == 0 &&
          a->transitions[i].from_etat == a->etats[u]) {
        int v = get_state_index(a, a->transitions[i].to_etat);
        if (v != -1 && !closure[v]) {
          closure[v] = true;
          stack[++top] = v;
        }
      }
    }
  }
}

void supprimer_epsilon_transitions(Automaton *a) {
  Transition new_transitions[MAX_TRANSITIONS];
  int new_count = 0;
  bool new_is_final[MAX_ETATS];
  memcpy(new_is_final, a->is_final, sizeof(new_is_final));

  for (int i = 0; i < a->num_etats; i++) {
    bool closure[MAX_ETATS];
    get_epsilon_closure(a, i, closure);

    // Mise à jour des états finaux
    for (int j = 0; j < a->num_etats; j++) {
      if (closure[j] && a->is_final[j]) {
        new_is_final[i] = true;
      }
    }

    // Duplication des transitions
    for (int j = 0; j < a->num_etats; j++) {
      if (closure[j]) {
        for (int t = 0; t < a->num_transitions; t++) {
          if (strcmp(a->transitions[t].label, "epsilon") != 0 &&
              a->transitions[t].from_etat == a->etats[j]) {

            // Ajouter transition de a->etats[i] vers a->transitions[t].to_etat
            bool exists = false;
            for (int n = 0; n < new_count; n++) {
              if (new_transitions[n].from_etat == a->etats[i] &&
                  new_transitions[n].to_etat == a->transitions[t].to_etat &&
                  strcmp(new_transitions[n].label, a->transitions[t].label) ==
                      0) {
                exists = true;
                break;
              }
            }
            if (!exists) {
              if (new_count < MAX_TRANSITIONS) {
                new_transitions[new_count].from_etat = a->etats[i];
                new_transitions[new_count].to_etat = a->transitions[t].to_etat;
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

  // Mettre à jour l'automate
  a->num_transitions = new_count;
  for (int i = 0; i < new_count; i++) {
    a->transitions[i] = new_transitions[i];
  }
  for (int i = 0; i < a->num_etats; i++) {
    a->is_final[i] = new_is_final[i];
  }

  // Nettoyage de l'alphabet (optionnel mais recommandé pour la clarté)
  // On recalcule l'alphabet à partir des nouvelles transitions
  a->num_alphabet = 0;
  for (int i = 0; i < a->num_transitions; i++) {
    bool exists = false;
    for (int j = 0; j < a->num_alphabet; j++) {
      if (strcmp(a->alphabet[j], a->transitions[i].label) == 0) {
        exists = true;
        break;
      }
    }
    if (!exists && a->num_alphabet < MAX_ALPHABET) {
      strcpy(a->alphabet[a->num_alphabet++], a->transitions[i].label);
    }
  }
}
#define MAX_REGEX 4096

typedef char RegexMatrix[MAX_ETATS][MAX_ETATS][MAX_REGEX];

static int needs_parens(const char *r) {
    if (strlen(r) <= 1) return 0;

    /* Déjà entourée de parenthèses balancées ? */
    if (r[0] == '(' && r[strlen(r) - 1] == ')') {
        int depth = 0;
        for (int i = 0; r[i]; i++) {
            if (r[i] == '(') depth++;
            else if (r[i] == ')') depth--;
            if (depth == 0 && r[i + 1] != '\0') return 1;
        }
        return 0; /* parenthèses déjà balancées */
    }

    /* Contient un + au niveau racine → besoin de parenthèses */
    int depth = 0;
    for (int i = 0; r[i]; i++) {
        if (r[i] == '(') depth++;
        else if (r[i] == ')') depth--;
        else if (r[i] == '+' && depth == 0) return 1;
    }
    return 0;
}

/* Concaténation : ignore les eps, ajoute parenthèses si + à la racine */
static void concat_regex(char *dst, const char *a, const char *b) {
    int a_empty = (strlen(a) == 0 || strcmp(a, "eps") == 0);
    int b_empty = (strlen(b) == 0 || strcmp(b, "eps") == 0);

    if (a_empty && b_empty) { strcpy(dst, ""); return; }
    if (a_empty) { strcpy(dst, b); return; }
    if (b_empty) { strcpy(dst, a); return; }

    /* ── Simplification xx* → x* et x*x → x* ──────────────────── */
    int la = strlen(a), lb = strlen(b);

    /* Cas : a = "x" et b = "x*" → x* */
    if (b[lb - 1] == '*') {
        char b_base[MAX_REGEX];
        if (b[0] == '(' && b[lb - 2] == ')') {
            strncpy(b_base, b + 1, lb - 3);
            b_base[lb - 3] = '\0';
        } else {
            strncpy(b_base, b, lb - 1);
            b_base[lb - 1] = '\0';
        }
        if (strcmp(a, b_base) == 0) { strcpy(dst, b); return; }
    }

    /* Cas : a = "x*" et b = "x" → x* */
    if (a[la - 1] == '*') {
        char a_base[MAX_REGEX];
        if (a[0] == '(' && a[la - 2] == ')') {
            strncpy(a_base, a + 1, la - 3);
            a_base[la - 3] = '\0';
        } else {
            strncpy(a_base, a, la - 1);
            a_base[la - 1] = '\0';
        }
        if (strcmp(a_base, b) == 0) { strcpy(dst, a); return; }
    }

    /* Cas général avec parenthèses si nécessaire */
    char pa[MAX_REGEX + 4], pb[MAX_REGEX + 4];
    if (needs_parens(a)) snprintf(pa, sizeof(pa), "(%s)", a);
    else strcpy(pa, a);

    if (needs_parens(b)) snprintf(pb, sizeof(pb), "(%s)", b);
    else strcpy(pb, b);

    char tmp[MAX_REGEX * 2];
    snprintf(tmp, sizeof(tmp), "%s.%s", pa, pb);
    strcpy(dst, tmp);
}

/* Union : élimine doublons et cas vides */
static void union_regex(char *dst, const char *a, const char *b) {
    if (strlen(a) == 0) { strcpy(dst, b); return; }
    if (strlen(b) == 0) { strcpy(dst, a); return; }
    if (strcmp(a, b) == 0) { strcpy(dst, a); return; }
    
    char tmp[MAX_REGEX * 2];
    snprintf(tmp, sizeof(tmp), "%s+%s", a, b);
    strcpy(dst, tmp);
}

/* Étoile de Kleene */
static void star_regex(char *dst, const char *a) {
    if (strlen(a) == 0 || strcmp(a, "eps") == 0) {
        dst[0] = '\0'; 
        return;
    }
    
    int len = strlen(a);
    if (a[len - 1] == '*') { strcpy(dst, a); return; }
    
    if (len == 1) {
        snprintf(dst, MAX_REGEX, "%s*", a); 
        return;
    }
    
    if (a[0] == '(' && a[len - 1] == ')' && !needs_parens(a)) {
        snprintf(dst, MAX_REGEX, "%s*", a); 
        return;
    }
    
    snprintf(dst, MAX_REGEX, "(%s)*", a);
}

char *automaton_to_regex(Automaton *a) {
    static RegexMatrix R;
    static char result[MAX_REGEX];
    static Automaton copy;

    memcpy(&copy, a, sizeof(Automaton));
    supprimer_epsilon_transitions(&copy);

    int n = copy.num_etats;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            R[i][j][0] = '\0';

    for (int t = 0; t < copy.num_transitions; t++) {
        int i = get_state_index(&copy, copy.transitions[t].from_etat);
        int j = get_state_index(&copy, copy.transitions[t].to_etat);
        if (i == -1 || j == -1) continue;
        char tmp[MAX_REGEX];
        union_regex(tmp, R[i][j], copy.transitions[t].label);
        strcpy(R[i][j], tmp);
    }

    int init = -1;
    for (int i = 0; i < n; i++)
        if (copy.is_initial[i]) { init = i; break; }
    
    if (init == -1) { strcpy(result, "∅"); return result; }

    /* Fusionner les chemins init→k→init */
    for (int k = 0; k < n; k++) {
        if (k == init) continue;
        if (strlen(R[init][k]) == 0) continue;
        if (strlen(R[k][init]) == 0) continue;

        char loop_k[MAX_REGEX], path[MAX_REGEX], tmp[MAX_REGEX];
        star_regex(loop_k, R[k][k]);
        concat_regex(tmp, R[init][k], loop_k);
        concat_regex(path, tmp, R[k][init]);
        union_regex(tmp, R[init][init], path);
        strcpy(R[init][init], tmp);
    }

    /* Élimination des états intermédiaires */
    bool eliminated[MAX_ETATS] = {false};
    for (int k = 0; k < n; k++) {
        if (k == init || copy.is_final[k] || eliminated[k]) continue;
        
        char loop[MAX_REGEX];
        star_regex(loop, R[k][k]);
        
        for (int i = 0; i < n; i++) {
            if (eliminated[i] || i == k || strlen(R[i][k]) == 0) continue;
            for (int j = 0; j < n; j++) {
                if (eliminated[j] || j == k || strlen(R[k][j]) == 0) continue;
                char path[MAX_REGEX], tmp[MAX_REGEX];
                concat_regex(tmp, R[i][k], loop);
                concat_regex(path, tmp, R[k][j]);
                union_regex(tmp, R[i][j], path);
                strcpy(R[i][j], tmp);
            }
        }
        eliminated[k] = true;
    }

    /* Construction du résultat final */
    result[0] = '\0';
    for (int f = 0; f < n; f++) {
        if (!copy.is_final[f] || eliminated[f]) continue;
        
        char path[MAX_REGEX], tmp[MAX_REGEX], loop_f[MAX_REGEX];
        star_regex(loop_f, R[f][f]);
        
        if (f == init) {
            char loop_init[MAX_REGEX];
            star_regex(loop_init, R[init][init]);
            union_regex(tmp, result, strlen(loop_init) ? loop_init : "eps");
        } else {
            char loop_init[MAX_REGEX];
            star_regex(loop_init, R[init][init]);
            concat_regex(path, loop_init, R[init][f]);
            concat_regex(tmp, path, loop_f);
            char res_tmp[MAX_REGEX];
            union_regex(res_tmp, result, tmp);
            strcpy(tmp, res_tmp);
        }
        strcpy(result, tmp);
    }

    if (result[0] == '\0') strcpy(result, "∅");
    return result;
}

Automaton *produit_automates(const Automaton *a1, const Automaton *a2) {
    Automaton *res = create_automaton();
    int max_id_a2 = 0;

    for (int i = 0; i < a2->num_etats; i++)
        if (a2->etats[i] > max_id_a2)
            max_id_a2 = a2->etats[i];

    int base = max_id_a2 + 1;
    int queue[MAX_ETATS];
    int front = 0, rear = 0;

    int init = a1->etats[0] * base + a2->etats[0];
    res->etats[0] = init;
    res->is_initial[0] = true;
    res->is_final[0] = a1->is_final[0] && a2->is_final[0];
    res->num_etats = 1;

    queue[rear++] = init;

    while (front < rear) {
        int current = queue[front++];
        int s1 = current / base;
        int s2 = current % base;

        for (int i = 0; i < a1->num_transitions; i++) {
            for (int j = 0; j < a2->num_transitions; j++) {
                if (strcmp(a1->transitions[i].label, a2->transitions[j].label) != 0)
                    continue;
                if (strcmp(a1->transitions[i].label, "epsilon") == 0)
                    continue;
                if (a1->transitions[i].from_etat != s1 || a2->transitions[j].from_etat != s2)
                    continue;

                int dest = a1->transitions[i].to_etat * base + a2->transitions[j].to_etat;

                int index = -1;
                for (int k = 0; k < res->num_etats; k++) {
                    if (res->etats[k] == dest) {
                        index = k;
                        break;
                    }
                }

                if (index == -1) {
                    index = res->num_etats;
                    res->etats[index] = dest;
                    res->is_initial[index] = false;

                    int t1 = a1->transitions[i].to_etat;
                    int t2 = a2->transitions[j].to_etat;
                    int idx1 = get_state_index(a1, t1);
                    int idx2 = get_state_index(a2, t2);

                    res->is_final[index] = a1->is_final[idx1] && a2->is_final[idx2];
                    res->num_etats++;
                    queue[rear++] = dest;
                }

                Transition *t = &res->transitions[res->num_transitions++];
                t->from_etat = current;
                t->to_etat = dest;
                strcpy(t->label, a1->transitions[i].label);
            }
        }
    }
    return res;
}

Automaton *determiniser(const Automaton *nfa) {
    Automaton *dfa = create_automaton();
    int sets[MAX_ETATS][MAX_ETATS];
    int set_sizes[MAX_ETATS] = {0};
    int num_sets = 0;

    // Etat initial
    for (int i = 0; i < nfa->num_etats; i++) {
        if (nfa->is_initial[i]) {
            sets[0][set_sizes[0]++] = i;
        }
    }

    dfa->etats[0] = 0;
    dfa->is_initial[0] = true;
    dfa->is_final[0] = false; // Sera vérifié après
    num_sets = 1;
    dfa->num_etats = 1;

    for (int s = 0; s < num_sets; s++) {
        for (int a = 0; a < nfa->num_alphabet; a++) {
            int new_set[MAX_ETATS];
            int new_size = 0;

            for (int i = 0; i < set_sizes[s]; i++) {
                int state = sets[s][i];
                for (int t = 0; t < nfa->num_transitions; t++) {
                    if (nfa->transitions[t].from_etat == nfa->etats[state] &&
                        strcmp(nfa->transitions[t].label, nfa->alphabet[a]) == 0) {
                        int idx = get_state_index(nfa, nfa->transitions[t].to_etat);
                        
                        // Éviter les doublons dans le nouveau set
                        bool exists = false;
                        for(int m=0; m<new_size; m++) if(new_set[m] == idx) exists = true;
                        if(!exists) new_set[new_size++] = idx;
                    }
                }
            }

            if (new_size == 0) continue;

            int found = -1;
            for (int k = 0; k < num_sets; k++) {
                if (set_sizes[k] == new_size) {
                    // Comparaison simplifiée (tri possible pour memcmp)
                    int match_count = 0;
                    for(int m1=0; m1<new_size; m1++)
                        for(int m2=0; m2<new_size; m2++)
                            if(sets[k][m1] == new_set[m2]) match_count++;
                    
                    if(match_count == new_size) {
                        found = k;
                        break;
                    }
                }
            }

            if (found == -1) {
                memcpy(sets[num_sets], new_set, sizeof(int) * new_size);
                set_sizes[num_sets] = new_size;
                dfa->etats[num_sets] = num_sets;
                
                bool is_final = false;
                for (int i = 0; i < new_size; i++) {
                    if (nfa->is_final[new_set[i]]) {
                        is_final = true;
                        break;
                    }
                }
                dfa->is_final[num_sets] = is_final;
                found = num_sets;
                num_sets++;
            }

            Transition *tr = &dfa->transitions[dfa->num_transitions++];
            tr->from_etat = s;
            tr->to_etat = found;
            strcpy(tr->label, nfa->alphabet[a]);
        }
    }
    dfa->num_etats = num_sets;
    return dfa;
}
Automaton *transposer_automaton(const Automaton *a) {
    Automaton *r = create_automaton();
    if (!r) return NULL;

    //  Copier l'alphabet
    r->num_alphabet = a->num_alphabet;
    for (int i = 0; i < a->num_alphabet; i++) {
        strcpy(r->alphabet[i], a->alphabet[i]);
    }

    // Copier les états en swappant initiaux ↔ finaux
    for (int i = 0; i < a->num_etats; i++) {
        r->etats[i]      = a->etats[i];
        r->is_initial[i] = a->is_final[i];
        r->is_final[i]   = a->is_initial[i];
    }
    r->num_etats = a->num_etats;

    // Inverser le sens de toutes les transitions
    for (int t = 0; t < a->num_transitions; t++) {
        Transition tr;
        tr.from_etat = a->transitions[t].to_etat;
        tr.to_etat   = a->transitions[t].from_etat;
        strcpy(tr.label, a->transitions[t].label);
        r->transitions[r->num_transitions++] = tr;
    }

    return r;
}
Automaton *minimiser_brzozowski(const Automaton *a) {
    if (!a) return NULL;

    // Étape 1 : Transposer
    printf("  Etape 1 : Transposer...\n");
    Automaton *r1 = transposer_automaton(a);
    if (!r1) return NULL;

    // ✅ Fusionner les états initiaux multiples en un seul
    // Compter les états initiaux
    int nb_initiaux = 0;
    for (int i = 0; i < r1->num_etats; i++)
        if (r1->is_initial[i]) nb_initiaux++;

    if (nb_initiaux > 1) {
        // Garder le premier initial, rediriger les transitions des autres
        int premier = -1;
        for (int i = 0; i < r1->num_etats; i++) {
            if (r1->is_initial[i]) {
                if (premier == -1) {
                    premier = i;
                } else {
                    // Rediriger toutes les transitions entrantes vers premier
                    for (int t = 0; t < r1->num_transitions; t++) {
                        if (r1->transitions[t].to_etat == r1->etats[i])
                            r1->transitions[t].to_etat = r1->etats[premier];
                        if (r1->transitions[t].from_etat == r1->etats[i])
                            r1->transitions[t].from_etat = r1->etats[premier];
                    }
                    r1->is_initial[i] = false; // désactiver
                }
            }
        }
    }

    // Étape 2 : Déterminiser
    printf("  Etape 2 : Determiniser...\n");
    Automaton *d1 = determiniser(r1);
    free_automaton(r1);
    if (!d1) return NULL;

    // Étape 3 : Transposer
    printf("  Etape 3 : Transposer...\n");
    Automaton *r2 = transposer_automaton(d1);
    free_automaton(d1);
    if (!r2) return NULL;

    // ✅ Même fusion pour r2
    nb_initiaux = 0;
    for (int i = 0; i < r2->num_etats; i++)
        if (r2->is_initial[i]) nb_initiaux++;

    if (nb_initiaux > 1) {
        int premier = -1;
        for (int i = 0; i < r2->num_etats; i++) {
            if (r2->is_initial[i]) {
                if (premier == -1) {
                    premier = i;
                } else {
                    for (int t = 0; t < r2->num_transitions; t++) {
                        if (r2->transitions[t].to_etat == r2->etats[i])
                            r2->transitions[t].to_etat = r2->etats[premier];
                        if (r2->transitions[t].from_etat == r2->etats[i])
                            r2->transitions[t].from_etat = r2->etats[premier];
                    }
                    r2->is_initial[i] = false;
                }
            }
        }
    }

    // Étape 4 : Déterminiser → Minimal
    printf("  Etape 4 : Determiniser...\n");
    Automaton *d2 = determiniser(r2);
    free_automaton(r2);
    if (!d2) return NULL;

    // ✅ Copier l'alphabet dans le résultat final
    d2->num_alphabet = a->num_alphabet;
    for (int i = 0; i < a->num_alphabet; i++)
        strcpy(d2->alphabet[i], a->alphabet[i]);

    printf("  Minimisation terminee !\n");
    return d2;
}