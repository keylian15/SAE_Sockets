# SAE_Sockets

Ce projet utilise des sockets pour implémenter une communication entre un serveur et un client. 
Dans le but de coder un T3N (Tic Tac Toe Network)
### Versions Finies :
`V0` - Jeu sans regles client serveur
`V1` - Jeu avec regles client serveur
`V2` - Jeu avec regles client client serveur (arbritre)
`V3` - Jeu avec regles client client serveur (arbritre) spectateur (max 3) 
### Versions Non Finies :
`V4` - Jeu avec regles client client serveur (arbritre) spectateur (max 3) + Possibilité de plusieurs parties  

## Pour toutes les versions

### Compilation des fichiers

1. Accédez au répertoire `V0` ou `V1` ou `V2` ou `V3` ou `V4`: 
   ```bash
   cd V0

2. Compiler et executer les fichiers vous avez deux solutions :
    Dans votre propre terminal (serveur)
   ```bash
   gcc -o morpion morpion.c
   gcc -o T3N_serveur_V3 T3N_serveur_V3.c
   gcc -o T3N_client_V3 T3N_client_V3.c
   gcc -o main main.c
   clear
   ./T3N_serveur_V3
3.
   Dans un terminal generer (serveur)
   ```bash
   gcc -o morpion morpion.c
   gcc -o T3N_serveur_V3 T3N_serveur_V3.c
   gcc -o T3N_client_V3 T3N_client_V3.c
   gcc -o main main.c
   clear
   ./main
