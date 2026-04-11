#include "../include/automate.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main() {
    int choix;
    char filename[256];
    Automaton *automate = create_automaton();
    bool loaded = false;

    printf("=== Projet Theorie des Langages ===\n");

    while (1) {
        printf("\nMenu:\n");
        printf("1. Charger un automate a partir d'un fichier .dot\n");
        printf("2. Afficher l'automate\n");
        printf("3. Generer un fichier .dot\n");
        printf("4. Afficher l'etat avec le plus de transitions entrantes/sortantes\n");
        printf("5. Afficher les etats avec une transition sortante d'une etiquette donnee\n");
        printf("6. Tester un mot saisi au clavier\n");
        printf("7. Analyser un fichier .txt et generer MotsAccepter.txt\n");
        printf("9. Concatener deux automates (.dot)\n");
        printf("10. Union de deux automates (.dot)\n");
        printf("11. Expression reguliere vers NFA\n");
        printf("12. Supprimer les transitions epsilon\n");
        printf("13. Automate vers expression reguliere\n");
        printf("14. Produit de deux automates (.dot)\n");
        printf("15. Determiniser l'automate\n");
        printf("16. Minimiser l'automate (Brzozowski)\n");
        printf("17. Analyser un fichier .dot \n");
        printf("18. Generer les fichiers .txt afficher liste des mots acceptes\n");
        printf("19. Generer un tableau de symbole\n");
        printf("20. Traiter depuis un fichier regex (Partie 6)\n");
        printf("21. Quitter\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix) != 1) {
            // Nettoyage de l'entrée en cas de frappe invalide
            while (getchar() != '\n');
            continue;
        }

        switch (choix) {
            case 1:
                printf("Entrez le nom du fichier (ex: data/test.dot) : ");
                scanf(" %255[^\n\r]", filename);
                free_automaton(automate);
                automate = create_automaton();
                if (load_automaton_from_dot(automate, filename)) {
                    printf("Automate charge avec succes depuis '%s'.\n", filename);
                    loaded = true;
                } else {
                    loaded = false;
                }
                break;

            case 2:
                if (loaded) {
                    display_automaton(automate);
                } else {
                    printf("Veuillez d'abord charger un automate (Option 1).\n");
                }
                break;

            case 3:
                printf("Entrez le nom du fichier de sortie pour le .dot genere : ");
                scanf(" %255[^\n\r]", filename);
                generate_dot_interactive(automate, filename);
                break;

            case 4:
                afficher_etat_max_transitions(automate);
                break;

            case 5:
                printf("Entrez l'etiquette de la transition sortante : ");
                char label[MAX_LABEL_LEN];
                scanf(" %63[^\n\r]", label);
                afficher_etats_transition_label(automate, label);
                break;

            case 6:
                lire_et_tester_mot(automate);
                break;

            case 7:
                printf("Entrez le nom du fichier d'entree (ex: mots_a_tester.txt) : ");
                char input_file[256];
                scanf(" %255[^\n\r]", input_file);
                filtrer_mots_fichier(automate, input_file);
                break;

            case 9:
            case 10: {
                char file1[256], file2[256];
                Automaton *a1 = create_automaton();
                Automaton *a2 = create_automaton();
                printf("Entrez le chemin du premier automate (.dot) : ");
                scanf(" %255[^\n\r]", file1);
                printf("Entrez le chemin du deuxieme automate (.dot) : ");
                scanf(" %255[^\n\r]", file2);
                
                if (load_automaton_from_dot(a1, file1) && load_automaton_from_dot(a2, file2)) {
                    Automaton *res;
                    if (choix == 9)
                        res = concatener_automates(a1, a2);
                    else
                        res = union_automates(a1, a2);

                    printf("\n--- Resultat ---\n");
                    display_automaton(res);

                    char out_file[256];
                    printf("Entrez le nom du fichier pour sauvegarder le resultat (.dot) : ");
                    if (scanf(" %255[^\n\r]", out_file) == 1) {
                        generate_dot(res, out_file);
                    }
                    free_automaton(res);
                } else {
                    printf("Erreur lors du chargement des fichiers.\n");
                }
                free_automaton(a1);
                free_automaton(a2);
                break;
            }

            case 11: {
                char regex_input[256];
                printf("Entrez l'expression reguliere : ");
                scanf(" %255[^\n\r]", regex_input);
                Automaton *res = regex_to_nfa(regex_input);
                printf("\n--- Automate Genere ---\n");
                display_automaton(res);

                char out_file[256];
                printf("Entrez le nom du fichier pour sauvegarder le resultat (.dot) : ");
                if (scanf(" %255[^\n\r]", out_file) == 1) {
                    generate_dot(res, out_file);
                }
                free_automaton(res);
                break;
            }

            case 12:
                if (!loaded) {
                    printf("Veuillez d'abord charger un automate (Option 1).\n");
                    break;
                }
                supprimer_epsilon_transitions(automate);
                printf("\nTransitions epsilon supprimees avec succes.\n");
                display_automaton(automate);

                char save_file[256];
                printf("Entrez le nom du fichier pour sauvegarder (.dot) : ");
                if (scanf(" %255[^\n\r]", save_file) == 1) {
                    generate_dot(automate, save_file);
                }
                break;

            case 8: // Redirection historique
            case 13:
                if (!loaded) {
                    printf("Veuillez d'abord charger un automate (Option 1).\n");
                    break;
                }
                {
                    char *res_regex = automaton_to_regex(automate);
                    if (res_regex) {
                        printf("\nExpression reguliere : %s\n", res_regex);
                        // Ne pas free si automaton_to_regex retourne un static, 
                        // mais si c'est un malloc, gardez le free.
                    } else {
                        printf("Erreur lors de la conversion.\n");
                    }
                }
                break;

            case 14:
                if (!loaded) {
                    printf("Veuillez d'abord charger l'automate 1 (Option 1).\n");
                    break;
                }
                {
                    char file2[256];
                    printf("Entrez le fichier du deuxieme automate (.dot) : ");
                    if (scanf(" %255[^\n\r]", file2) == 1) {
                        Automaton *automate2 = create_automaton();
                        if (load_automaton_from_dot(automate2, file2)) {
                            Automaton *produit = produit_automates(automate, automate2);
                            if (produit) {
                                printf("\n=== Automate Produit ===\n");
                                display_automaton(produit);
                                char save_file[256];
                    printf("Entrez le nom du fichier pour sauvegarder (.dot) : ");
                if (scanf(" %255[^\n\r]", save_file) == 1) {
                    generate_dot(produit, save_file);
                }
                                free_automaton(produit);
                            }
                            
                        }
                        free_automaton(automate2);
                    }
                }
                break;

          case 15:
    if (!loaded) {
        printf("Veuillez d'abord charger un automate (Option 1).\n");
        break;
    }

    char file_dfa[256];
    printf("Entrez le fichier de l'automate a determiniser (.dot) : ");
    if (scanf(" %255[^\n\r]", file_dfa) == 1) {
        Automaton *automate_copy = create_automaton();
        if (load_automaton_from_dot(automate_copy, file_dfa)) {

            printf("\nSuppression des epsilons avant determinisation...\n");
            supprimer_epsilon_transitions(automate_copy);

            Automaton *dfa = determiniser(automate_copy);
            if (dfa) {
                printf("\n=== Automate Deterministe (DFA) ===\n");
                display_automaton(dfa);

                char save_file[256];
                printf("Entrez le nom du fichier pour sauvegarder (.dot) : ");
                if (scanf(" %255[^\n\r]", save_file) == 1) {
                    generate_dot(dfa, save_file);
                }

                // Remplacer l'automate courant par le DFA
                free_automaton(automate);
                automate = dfa;
                loaded = 1;
                printf("\nAutomate courant remplace par le DFA.\n");
            } else {
                printf("Echec de la determinisation.\n");
            }

            free_automaton(automate_copy);
        } else {
            printf("Erreur : impossible de charger %s\n", file_dfa);
            free_automaton(automate_copy);
        }
    }
    break;
   case 16:
    if (!loaded) {
        printf("Veuillez d'abord charger un automate (Option 1).\n");
        break;
    }

    char file_brz[256];
    printf("Entrez le fichier de l'automate a minimiser (.dot) : ");
    if (scanf(" %255[^\n\r]", file_brz) == 1) {
        Automaton *automate_brz = create_automaton();
        if (load_automaton_from_dot(automate_brz, file_brz)) {

            printf("\n=== Automate Original ===\n");
            display_automaton(automate_brz);

            char save_file[256];
                printf("Entrez le nom du fichier pour sauvegarder (.dot) : ");
                if (scanf(" %255[^\n\r]", save_file) == 1) {
                    generate_dot(automate_brz, save_file);
                }

            printf("\nMinimisation par algorithme de Brzozowski...\n");
            Automaton *minimal = minimiser_brzozowski(automate_brz);

            if (minimal) {
                printf("\n=== Automate Minimal (Brzozowski) ===\n");
                display_automaton(minimal);

                char save_file[256];
                printf("Entrez le nom du fichier pour sauvegarder (.dot) : ");
                if (scanf(" %255[^\n\r]", save_file) == 1) {
                    generate_dot(minimal, save_file);
                }

                // Remplacer l'automate courant par le minimal
                free_automaton(automate);
                automate = minimal;
                loaded = 1;
                printf("\nAutomate courant remplace par l'automate minimal.\n");
            } else {
                printf("Echec de la minimisation.\n");
            }

            free_automaton(automate_brz);
        } else {
            printf("Erreur : impossible de charger %s\n", file_brz);
            free_automaton(automate_brz);
        }
    }
    break;
    case 17:
    if (!loaded) {
        printf("Veuillez d'abord charger un automate (Option 1).\n");
        break;
    }

    char nom_base[256];
    printf("Entrez le nom de base des fichiers (ex: mon_automate) : ");
    if (scanf(" %255[^\n\r]", nom_base) == 1) {
        printf("\nGeneration des fichiers .dot...\n");
        generer_dot_pipeline(automate, nom_base);
        printf("\nFichiers generes avec succes !\n");
    }
    break;
    case 18:
    if (!loaded) {
        printf("Veuillez d'abord charger un automate (Option 1).\n");
        break;
    }

    char file_txt[256];
    printf("Entrez le fichier .dot de l'automate a minimiser : ");
    char file_min18[256];
    scanf(" %255[^\n\r]", file_min18);

    printf("Entrez le fichier .txt contenant les mots : ");
    if (scanf(" %255[^\n\r]", file_txt) == 1) {
        Automaton *automate_18 = create_automaton();
        if (load_automaton_from_dot(automate_18, file_min18)) {

            printf("\nMinimisation en cours...\n");
            Automaton *minimal = minimiser_brzozowski(automate_18);

            if (minimal) {
                printf("\n=== Automate Minimal ===\n");
                display_automaton(minimal);

                afficher_mots_acceptes(minimal, file_txt);
                free_automaton(minimal);
            } else {
                printf("Echec de la minimisation.\n");
            }
        } else {
            printf("Erreur : impossible de charger %s\n", file_min18);
        }
        free_automaton(automate_18);
    }
    break;
            case 19: {
                Automaton *a = regex_to_nfa("a.b|a");
                supprimer_epsilon_transitions(a);
                Automaton *dfa = determiniser(a);
                char mots_file[256];
                printf("Entrez le fichier texte a analyser : ");
                scanf(" %255[^\n\r]", mots_file);
                generer_table_symboles(dfa, mots_file);
                free_automaton(a);
                free_automaton(dfa);
                break;
            }
            case 20: {
                char f_regex[256];
                char f_mots[256];
                printf("Entrez le fichier texte contenant l'expression reguliere : ");
                scanf(" %255[^\n\r]", f_regex);
                printf("Entrez le fichier texte a analyser (contenant les lexemes) : ");
                scanf(" %255[^\n\r]", f_mots);
                traiter_partie_6(f_regex, f_mots);
                break;
            }
            case 21:
                printf("Au revoir !\n");
                free_automaton(automate);
                return 0;

            default:
                printf("Choix invalide. Veuillez reessayer.\n");
                break;
        }
    }
    return 0;
}