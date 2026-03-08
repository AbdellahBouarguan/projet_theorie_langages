#include "../include/automate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// État avec le plus de transitions entrantes/sortantes
void afficher_etat_max_transitions(const Automaton *automate)
{
    if (!automate || automate->num_etats == 0)
    {
        printf("L'automate est vide.\n");
        return;
    }

    int in_count[MAX_ETATS] = {0};
    int out_count[MAX_ETATS] = {0};

    // Comptage
    for (int i = 0; i < automate->num_transitions; i++)
    {
        for (int j = 0; j < automate->num_etats; j++)
        {
            if (automate->etats[j] == automate->transitions[i].from_etat)
                out_count[j]++;
            if (automate->etats[j] == automate->transitions[i].to_etat)
                in_count[j]++;
        }
    }

    int max_in = -1, max_out = -1;
    int etat_max_in = -1, etat_max_out = -1;

    // Recherche des maximums
    for (int i = 0; i < automate->num_etats; i++)
    {
        if (in_count[i] > max_in)
        {
            max_in = in_count[i];
            etat_max_in = automate->etats[i];
        }
        if (out_count[i] > max_out)
        {
            max_out = out_count[i];
            etat_max_out = automate->etats[i];
        }
    }

    printf("Etat avec le plus de transitions sortantes : q%d (%d transitions)\n", etat_max_out, max_out);
    printf("Etat avec le plus de transitions entrantes : q%d (%d transitions)\n", etat_max_in, max_in);
}

// États avec une transition sortante étiquetée par une lettre précise
void afficher_etats_transition_label(const Automaton *automate, const char *label)
{
    if (!automate)
        return;

    printf("Etats ayant une transition sortante avec l'etiquette '%s' : { ", label);
    bool found = false;
    bool printed[MAX_ETATS] = {false};

    for (int i = 0; i < automate->num_transitions; i++)
    {
        if (strcmp(automate->transitions[i].label, label) == 0)
        {
            int etat_source = automate->transitions[i].from_etat;

            // Trouver l'index de l'état pour vérifier s'il a déjà été affiché
            for (int j = 0; j < automate->num_etats; j++)
            {
                if (automate->etats[j] == etat_source && !printed[j])
                {
                    if (found)
                        printf(", ");
                    printf("q%d", etat_source);
                    printed[j] = true;
                    found = true;
                    break;
                }
            }
        }
    }

    if (!found)
        printf("Aucun");
    printf(" }\n");
}

// Fonction récursive cachée (backtracking) pour simuler un automate fini non déterministe (AFN)
static bool match_recursive(const Automaton *a, int etat_actuel, const char *mot, int pos, bool visited_eps[])
{
    // Si on a lu tout le mot
    if (pos == (int)strlen(mot))
    {
        // Est-ce un état final ?
        for (int i = 0; i < a->num_etats; i++)
        {
            if (a->etats[i] == etat_actuel && a->is_final[i])
                return true;
        }

        // Ou peut-on atteindre un état final via epsilon ?
        for (int i = 0; i < a->num_transitions; i++)
        {
            if (a->transitions[i].from_etat == etat_actuel && strcmp(a->transitions[i].label, "epsilon") == 0)
            {
                int next_etat = a->transitions[i].to_etat;
                for (int j = 0; j < a->num_etats; j++)
                {
                    if (a->etats[j] == next_etat && !visited_eps[j])
                    {
                        visited_eps[j] = true;
                        if (match_recursive(a, next_etat, mot, pos, visited_eps))
                            return true;
                        visited_eps[j] = false;
                    }
                }
            }
        }
        return false;
    }

    char char_to_match[2] = {mot[pos], '\0'};

    for (int i = 0; i < a->num_transitions; i++)
    {
        if (a->transitions[i].from_etat == etat_actuel)
        {
            int next_etat = a->transitions[i].to_etat;

            // Transition Epsilon
            if (strcmp(a->transitions[i].label, "epsilon") == 0)
            {
                for (int j = 0; j < a->num_etats; j++)
                {
                    if (a->etats[j] == next_etat && !visited_eps[j])
                    {
                        visited_eps[j] = true;
                        if (match_recursive(a, next_etat, mot, pos, visited_eps))
                            return true;
                        visited_eps[j] = false;
                    }
                }
            }
            // Transition correspondant à la lettre courante du mot
            else if (strcmp(a->transitions[i].label, char_to_match) == 0)
            {
                bool new_visited[MAX_ETATS] = {false}; // Reset des epsilons pour la prochaine lettre
                if (match_recursive(a, next_etat, mot, pos + 1, new_visited))
                    return true;
            }
        }
    }
    return false;
}

// Tester si un mot est reconnu par l'automate

bool tester_mot(const Automaton *automate, const char *mot)
{
    if (!automate)
        return false;

    // Lancer la recherche depuis chaque état initial
    for (int i = 0; i < automate->num_etats; i++)
    {
        if (automate->is_initial[i])
        {
            bool visited_eps[MAX_ETATS] = {false};
            visited_eps[i] = true;
            if (match_recursive(automate, automate->etats[i], mot, 0, visited_eps))
            {
                return true;
            }
        }
    }
    return false;
}

void lire_et_tester_mot(const Automaton *automate)
{
    char mot[256];
    printf("Entrez un mot a tester (utilisez 'epsilon' pour le mot vide) : ");
    scanf(" %255s", mot);

    // Traitement du mot vide si l'utilisateur tape le mot "epsilon"
    if (strcmp(mot, "epsilon") == 0)
        mot[0] = '\0';

    if (tester_mot(automate, mot))
    {
        printf("Resultat : Le mot est ACCEPTE par l'automate.\n");
    }
    else
    {
        printf("Resultat : Le mot est REFUSE par l'automate.\n");
    }
}

// Lire un fichier .txt et enregistrer les mots reconnus
void filtrer_mots_fichier(const Automaton *automate, const char *fichier_entree)
{
    if (!automate)
    {
        printf("Erreur : Automate non charge.\n");
        return;
    }

    FILE *fin = fopen(fichier_entree, "r");
    if (!fin)
    {
        printf("Erreur : Impossible d'ouvrir le fichier source '%s'.\n", fichier_entree);
        return;
    }

    FILE *fout = fopen("MotsAccepter.txt", "w");
    if (!fout)
    {
        printf("Erreur : Impossible de creer le fichier 'MotsAccepter.txt'.\n");
        fclose(fin);
        return;
    }

    char mot[256];
    int count_total = 0;
    int count_acceptes = 0;

    printf("Analyse du fichier '%s' en cours...\n", fichier_entree);

    while (fscanf(fin, "%255s", mot) == 1)
    {
        count_total++;
        if (tester_mot(automate, mot))
        {
            fprintf(fout, "%s\n", mot);
            count_acceptes++;
        }
    }

    fclose(fin);
    fclose(fout);

    printf("Analyse terminee : %d mots lus, %d mots acceptes.\n", count_total, count_acceptes);
    printf("Les mots reconnus ont ete sauvegardes dans 'MotsAccepter.txt'.\n");
}