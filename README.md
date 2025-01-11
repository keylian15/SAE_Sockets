# SAE_Sockets

Ce projet utilise des sockets pour implémenter une communication entre un serveur et un client. 
Dans le but de coder un T3N (Tic Tac Toe Network)
### Versions Finies :
`V0` - Jeu sans regles client serveur.
`V1` - Jeu avec regles client serveur.
`V2` - Jeu avec regles client client serveur (arbritre).
`V3` - Jeu avec regles client client serveur (arbritre) spectateur (max 3). 
### Versions Non Finies :
`V4` - Jeu avec regles client client serveur (arbritre) spectateur (max 3) + Possibilité de plusieurs parties. Actuellement nous avons des soucis avec les processus fils et la gestion du serveur.

## A NOTER :
1. Si vous souhaiter gerer vous même l'excution de chaque client il vous suffit de commenter la ligne qui suit ceci : `// Commentaire a rajouter si vous voulez une execution manuelle.` dans le fichier serveur de la version concernée;
2. Des `logs` sont en place afin de mieux comprendre l'avancement de chaque jeu. Si vous voulez les desactiver il vous suffit de commenter la ligne qui suit ceci : `// Commentaire a rajouter si vous ne souhaitez pas de logs.` dans les fonctions `verifRecu` et `verifEnvoye`.

### Prérequis : 

1. Installer `xterm`. Nous utilisaons le package `xterm` afin de generer nous même des termineaux et ne pas s'embeter avec 6 termineaux directement.
   ```bash
   sudo apt install xterm

## Pour `V0` : 

### Compilation des fichiers

1. Accédez au répertoire `V0`: 
   ```bash
   cd V0

2. Compiler les fichiers :
   ```bash
   gcc -o morpion morpion.c
   gcc -o T3N_serveur_V0 T3N_serveur_V0.c
   gcc -o T3N_client_V0 T3N_client_V0.c
   gcc -o main main.c
   clear
3. Lancer de deux facons différentes (Une avec l'utilisation du terminal créer, l'autre qui va en generer un): 
   ```bash
   ./T3N_serveur_V0
   ./main

## Pour `V1` : 

### Compilation des fichiers

1. Accédez au répertoire `V1`: 
   ```bash
   cd V1

2. Compiler les fichiers :
   ```bash
   gcc -o morpion morpion.c
   gcc -o T3N_serveur_V1 T3N_serveur_V1.c
   gcc -o T3N_client_V1 T3N_client_V1.c
   gcc -o main main.c
   clear

3. Lancer de deux facons différentes : 
   ```bash
   ./T3N_serveur_V1
   ./main

## Pour `V2` : 

### Compilation des fichiers

1. Accédez au répertoire `V2`: 
   ```bash
   cd V2

2. Compiler les fichiers :
   ```bash
   gcc -o morpion morpion.c
   gcc -o T3N_serveur_V2 T3N_serveur_V2.c
   gcc -o T3N_client_V2 T3N_client_V2.c
   gcc -o main main.c
   clear

3. Lancer de deux facons différentes : 
   ```bash
   ./T3N_serveur_V2
   ./main

## Pour `V3` : 

### Compilation des fichiers

1. Accédez au répertoire `V3`: 
   ```bash
   cd V3

2. Compiler les fichiers :
   ```bash
   gcc -o morpion morpion.c
   gcc -o T3N_serveur_V3 T3N_serveur_V3.c
   gcc -o T3N_client_V3 T3N_client_V3.c
   gcc -o main main.c
   clear

3. Lancer de deux facons différentes : 
   ```bash
   ./T3N_serveur_V3
   ./main

## Pour `V4` : 

### Compilation des fichiers

1. Accédez au répertoire `V4`: 
   ```bash
   cd V4

2. Compiler les fichiers :
   ```bash
   gcc -o morpion morpion.c
   gcc -o T3N_serveur_V4 T3N_serveur_V4.c
   gcc -o T3N_client_V4 T3N_client_V4.c
   gcc -o main main.c
   clear

3. Lancer de deux facons différentes : 
   ```bash
   ./T3N_serveur_V34
   ./main
