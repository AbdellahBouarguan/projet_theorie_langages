# Projet Théorie des Langages et Compilation

Bienvenue sur le dépôt du projet de **Théorie des Langages et Compilation** (École Nationale des Sciences Appliquées - Tétouan).

L'objectif global de ce projet est de développer un programme en permettant de remplir une partie de la table des symboles à partir soit d'une expression régulière, soit d'un automate fini décrit sous forme d'un fichier `.dot` (format GraphviZ).

Ce dépôt contient l'implémentation de la **Partie 1** du projet :

- Chargement d'un automate fini à partir d'un fichier `.dot`.
- Analyse (parsing) du fichier pour en extraire et structurer les états, les transitions, l'alphabet et la nature des états (initial, final).
- Affichage clair et structuré des informations détaillées de l'automate dans la console sous forme de menu interactif.

## 👥 L'Équipe

Ce travail a été réalisé de manière collaborative (concept de _Feature Branching_ via Git) par :

- **Bouarguan Abdellah**
- **Ben Yacoub Nizar**
- **El Ghazouani Marouane**
- **Cherradi Ilyass**
- **El Younoussi Nafisa**

---

## 🛠️ Prérequis

Afin de compiler et d'exécuter ce projet, vous devez disposer des outils suivants, selon votre système d'exploitation :

### Sous Linux (Ubuntu, Manjaro, Arch, etc.)

- Un compilateur C (`gcc` ou `clang`).
- **CMake** (version 3.10 ou supérieure).
- _Optionnel_ : **GraphviZ** (paquet `graphviz`) pour visualiser les automates via la commande en ligne `dot`.

### Sous Windows

- **CMake** (version 3.10 ou supérieure).
- Un compilateur C compatible :
  - Soit **MSVC** (Microsoft Visual C++ fourni avec Visual Studio Build Tools).
  - Soit **MinGW-w64** (installable via MSYS2 par exemple).
- _Optionnel_ : L'outil en ligne [GraphvizOnline](https://dreampuf.github.io/GraphvizOnline/) pour visualiser directement les fichiers `.dot` de test de manière graphique sans installation.

---

## 🚀 Compilation et Exécution

Notre projet utilise exclusivement le système de build automatisé **CMake** pour garantir une compilation portable (agnostique au système d'exploitation).

### 🐧 Instructions de compilation sous Linux

1. Clonez ce dépôt et naviguez dans le répertoire racine :

   ```bash
   git clone https://github.com/AbdellahBouarguan/projet_theorie_langages.git
   cd projet_theorie_langages
   ```

2. Créez un répertoire isolé pour la compilation (`build`), configurez, puis compilez :

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. Exécutez le programme (l'exécutable sera généré automatiquement dans le répertoire virtuel `bin/` pointant vers la racine) :
   ```bash
   cd ..
   ./build/bin/programme_automate
   ```

### 🪟 Instructions de compilation sous Windows

1. Clonez ce dépôt via Git CMD, PowerShell ou Git Bash :

   ```cmd
   git clone https://github.com/AbdellahBouarguan/projet_theorie_langages.git
   cd projet_theorie_langages
   ```

2. Créez le dossier `build` et effectuez la configuration CMake :

   ```cmd
   mkdir build
   cd build
   cmake ..
   ```

   _(CMake détectera automatiquement votre compilateur local. Si vous utilisez spécifiquement MinGW, vous pouvez forcer le générateur : `cmake -G "MinGW Makefiles" ..`)_

3. Lancez le processus de compilation natif Windows via CMake :

   ```cmd
   cmake --build .
   ```

4. Exécutez le programme. L'emplacement de l'exécutable dépend du compilateur utilisé (Debug avec MSVC ou bin direct) :
   ```cmd
   .\bin\programme_automate.exe
   ```
   _(Note: Si vous avez compilé avec MSVC, le chemin peut-être : `.\bin\Debug\programme_automate.exe`)_

---

## 📂 Structure du projet

- `src/` : Fichiers sources en langage C (`main.c`, `parser.c`, `display.c`).
- `include/` : Fichiers d'en-tête contenant nos modèles de données (`automate.h`).
- `data/` : Répertoire contenant des fichiers textes de tests au format `.dot` (utilisez-les comme inputs pour tester le parsing).
- `docs/` : Dossier contenant notre rapport LaTeX exhaustif sur nos choix d'ingénierie ainsi que son rendu PDF directement consultable (`rapport.pdf`).
- `CMakeLists.txt` : Le fichier de configuration moteur pour la génération des scripts de compilation natifs.
