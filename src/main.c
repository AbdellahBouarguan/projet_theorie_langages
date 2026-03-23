#include "../include/automate.h"
#include <stdio.h>

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
    printf("4. Afficher l'etat avec le plus de transitions entrantes/sortantes "
           "\n");
    printf("5. Afficher les etats avec une transition sortante d'une etiquette "
           "donnee \n");
    printf("6. Tester un mot saisi au clavier \n");
    printf("7. Analyser un fichier .txt et generer MotsAccepter.txt\n");
    printf("9. Concatener deux automates (.dot)\n");
    printf("10. Union de deux automates (.dot)\n");
    printf("11. Expression reguliere vers NFA\n");
    printf("12. Supprimer les transitions epsilon\n");
    printf("13. Quitter\n");
    printf("Votre choix : ");

    if (scanf("%d", &choix) != 1) {
      // Nettoyage de l'entrée en cas de frappe invalide
      while (getchar() != '\n')
        ;
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
      printf("Entrez le nom du fichier de sortie pour le .dot genere (ex: "
             "output.dot) : ");
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
      if (load_automaton_from_dot(a1, file1) &&
          load_automaton_from_dot(a2, file2)) {
        Automaton *res;
        if (choix == 9)
          res = concatener_automates(a1, a2);
        else
          res = union_automates(a1, a2);

        printf("\n--- Resultat ---\n");
        display_automaton(res);

        char out_file[256];
        printf(
            "Entrez le nom du fichier pour sauvegarder le resultat (.dot) : ");
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
      char regex[256];
      printf("Entrez l'expression reguliere : ");
      scanf(" %255[^\n\r]", regex);
      Automaton *res = regex_to_nfa(regex);
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
    case 12: {
      if (!loaded) {
        printf("Veuillez d'abord charger un automate (Option 1) sur lequel "
               "executer l'algorithme.\n");
        break;
      }
      supprimer_epsilon_transitions(automate);
      printf("\nTransitions epsilon supprimees de l'automate principal avec "
             "succes.\n");
      display_automaton(automate);

      char out_file[256];
      printf("Entrez le nom du fichier pour sauvegarder le resultat (.dot) : ");
      if (scanf(" %255[^\n\r]", out_file) == 1) {
        generate_dot(automate, out_file);
      }
      break;
    }
    case 8: // Ancien Quitter redirigé
    case 13:
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