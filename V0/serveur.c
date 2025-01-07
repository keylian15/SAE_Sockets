#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "morpion.h" // Inclure le fichier d'en-tête pour utiliser les fonctions du jeu
#include "morpion.c"
#define PORT 5000
#define LG_MESSAGE 256

int main()
{
    int socketEcoute, socketDialogue;
    struct sockaddr_in pointDeRencontreLocal, pointDeRencontreDistant;
    socklen_t longueurAdresse;

    // ====== Création socket (1) ======
    socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
    if (socketEcoute < 0)
    {
        perror("Erreur en création de la socket...");
        exit(-1);
    }
    printf("Socket d'écoute créée! (%d)\n", socketEcoute);

    // ====== Configuration Attachement Local (2) ======
    memset(&pointDeRencontreLocal, 0x00, sizeof(pointDeRencontreLocal));
    pointDeRencontreLocal.sin_family = AF_INET;
    pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    pointDeRencontreLocal.sin_port = htons(PORT);
    if (bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, sizeof(pointDeRencontreLocal)) < 0)
    {
        perror("Erreur d'attachement de l'adresse locale...");
        close(socketEcoute);
        exit(-2);
    }
    printf("Adresse locale attachée.\n");

    // ====== Déclaration Nombre Maximum Connexions (3) ======
    if (listen(socketEcoute, 5) < 0)
    {
        perror("Erreur lors de la mise en écoute...");
        close(socketEcoute);
        exit(-3);
    }
    printf("En écoute sur le port %d...\n", PORT);

    while (1)
    {
        // ====== Attente Demande Connection (4) ======
        printf("En attente d'une connexion...\n");
        longueurAdresse = sizeof(pointDeRencontreDistant);
        socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
        if (socketDialogue < 0)
        {
            perror("Erreur lors de l'acceptation...");
            close(socketEcoute);
            exit(-4);
        }
        printf("Connexion acceptée.\n");

        // Jeu
        jeuServeur(socketDialogue);
    }
    // ====== Fermeture Socket Ecoute (9) ======
    close(socketEcoute);
    printf("Socket d'écoute fermée.\n");

    return 0;
}
void jeuServeur(int socketDialogue)
{
    // Déclaration des variables.
    int nb;
    char messageRecu[LG_MESSAGE];

    Morpion jeu;
    initialise(&jeu);

    // ====== Envoi Message de départ ======
    char message[LG_MESSAGE] = "start";
    send(socketDialogue, message, strlen(message) + 1, 0);

    // ====== Boucle de jeu ======
    while (1)
    {
        // ====== Réception Message Client ======
        memset(messageRecu, 0x00, LG_MESSAGE);
        nb = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
        if (nb <= 0)
        {
            perror("Erreur lors de la réception...");
            close(socketDialogue);
            continue;
        }
        // Conversion de la case choisie en int
        int case_client = atoi(messageRecu);

        // Verification choix.
        if (!isValid(case_client))
        {
            char erreur[LG_MESSAGE] = "erreur";
            send(socketDialogue, erreur, strlen(erreur) + 1, 0);
        }
        else
        {
            char confirm[LG_MESSAGE] = "confirm";
            send(socketDialogue, confirm, strlen(confirm) + 1, 0);

            // Placer la case du client coté serveur.
            place(&jeu, case_client, 'X');
            printf("Case client joué %d", case_client);

            // Le serveur joue en choisissant une case au hasard
            int case_serveur = rand() % 9 + 1;
            place(&jeu, case_serveur, 'O');

            // Envoie de la case serveur.
            char case_serveur_str[LG_MESSAGE];
            snprintf(case_serveur_str, LG_MESSAGE, "%d", case_serveur);
            send(socketDialogue, case_serveur_str, strlen(case_serveur_str) + 1, 0);

            // POUR LA V1
            // while (jeu.grille[(case_serveur - 1) / 3][(case_serveur - 1) % 3] != ' ')
            // {
            //     case_serveur = rand() % 9 + 1;
            // }
        }
    }

    // ====== Fermeture Socket Dialogue (8) ======
    close(socketDialogue);
    printf("Socket de dialogue fermée.\n");
}