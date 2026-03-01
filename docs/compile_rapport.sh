#!/bin/bash

# Se positionner dans le répertoire du script (dossier docs)
cd "$(dirname "$0")"

# Aller dans le répertoire contenant les sources LaTeX
cd latexRapport

# Compiler le fichier LaTeX (deux fois pour résoudre les références si nécessaire)
echo "Compilation du rapport en cours..."
pdflatex -interaction=nonstopmode rapport.tex > /dev/null
pdflatex -interaction=nonstopmode rapport.tex > /dev/null

# Vérifier si la compilation a réussi
if [ -f rapport.pdf ]; then
    # Déplacer le PDF généré directement dans le dossier docs/
    mv rapport.pdf ../rapport.pdf
    echo "Succès : Le rapport PDF a été généré dans le dossier docs/ (docs/rapport.pdf)"
else
    echo "Erreur : La compilation a échoué. Aucun PDF n'a été généré."
fi
