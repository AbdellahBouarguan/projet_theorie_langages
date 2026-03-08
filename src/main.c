#include "../include/automate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int choix;
    char filename[256];
    Automaton *automate = create_automaton();
    bool loaded = false;

    printf("=== Projet Theorie des Langages - Phase 1 ===\n");

    while (1)
    {
        printf("\nMenu:\n");
        printf("1. Charger un automate a partir d'un fichier .dot\n");
        printf("2. Afficher l'automate\n");
        printf("3. Generer un fichier .dot a partir de l'automate actuel\n");
        printf("4. Quitter\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix) != 1)
        {
            // Nettoyage de l'entrée en cas de frappe invalide
            while (getchar() != '\n')
                ;
            continue;
        }

        switch (choix)
        {
        case 1:
            printf("Entrez le nom du fichier (ex: data/test.dot) : ");
            scanf(" %255[^\n\r]", filename);
            free_automaton(automate);
            automate = create_automaton();
            if (load_automaton_from_dot(automate, filename))
            {
                printf("Automate charge avec succes depuis '%s'.\n", filename);
                loaded = true;
            }
            else
            {
                loaded = false;
            }
            break;
        case 2:
            if (loaded)
            {
                display_automaton(automate);
            }
            else
            {
                printf("Veuillez d'abord charger un automate (Option 1).\n");
            }
            break;
        case 3:
            printf("Entrez le nom du fichier de sortie pour le .dot genere (ex: output.dot) : ");
            scanf(" %255[^\n\r]", filename);
            generate_dot(automate, filename);
            printf("Automate genere dans le fichier '%s'.\n", filename);
            break;
        case 4:
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