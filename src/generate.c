#include "../include/automate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void generate_dot(Automaton *a, char *filename) {
FILE *f = fopen(filename, "w");
if(f == NULL) {
printf("Erreur création fichier.\n");
return;
}
printf("Génération du fichier %s...\n", filename);
printf("Entrer le nbre les états de l'automate : ");
scanf("%d", &a->num_etats);
for(int i=0; i<a->num_etats; i++) {
printf("Entrer l'identifiant de l'état %d : ", i);
scanf("%d", &a->etats[i]);
printf("L'état %d est-il initial ? (1 pour oui, 0 pour non) : ", a->etats[i]);
scanf("%d", &a->is_initial[i]);
printf("L'état %d est-il final ? (1 pour oui, 0 pour non) : ", a->etats[i]);
scanf("%d", &a->is_final[i]);
}
fprintf(f, "digraph Automaton {\n");
fprintf(f, "rankdir=LR;\n");
fprintf(f, "node [shape = circle];\n");
for(int i=0; i<a->num_etats; i++)
fprintf(f, "q%d;\n", a->etats[i]);
// Etat initial
fprintf(f, "start [shape=point];\n");
for(int i=0; i<a->num_etats; i++)
if(a->is_initial[i])
fprintf(f, "start -> q%d;\n", a->etats[i]);
// Transitions normales
printf("Entrer le nbre de transitions de l'automate : ");
scanf("%d", &a->num_transitions);
for(int i=0; i<a->num_transitions; i++) {
printf("Transition %d :\n", i+1);
printf("Etat de départ : ");
scanf("%d", &a->transitions[i].from_etat);
printf("Etat d'arrivée : ");
scanf("%d", &a->transitions[i].to_etat);
printf("Label de la transition : ");
scanf(" %s", a->transitions[i].label);
}
for(int i=0; i<a->num_transitions; i++)
fprintf(f, "q%d -> q%d [label=\"%s\"];\n",
a->transitions[i].from_etat,
a->transitions[i].to_etat,
a->transitions[i].label);
// Etats finaux : flèche sortante
fprintf(f, "node [shape=point];\n");
for(int i=0; i<a->num_etats; i++)
if(a->is_final[i])
fprintf(f, "q%d -> final%d;\n", a->etats[i], a->etats[i]);
fprintf(f, "}\n");
fclose(f);
printf("Fichier %s généré selon notation du cours.\n", filename);
}