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
    int socketEcoute, socketClient1, socketClient2;
    struct sockaddr_in pointDeRencontreLocal, pointDeRencontreDistant;
    socklen_t longueurAdresse;
    int nbSpectateurMax;
    bool conditionNbJoueur;
    // ====== Demande Nombre Spectateur ======
    conditionNbJoueur = false;
    while (!conditionNbJoueur)
    {
        printf("Entrez le nombre de spectateur (0 - 3) : \n");
        scanf(" %d", &nbSpectateurMax);
        printf("Vous avez choisi %d.\n", nbSpectateurMax);
        if (nbSpectateurMax > -1 && nbSpectateurMax < 4)
        {
            conditionNbJoueur = true;
        }
    }
    // ====== Fin Demande Nombre Spectateur ======

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
        printf("En attente de la connexion du joueur 1...\n");
        longueurAdresse = sizeof(pointDeRencontreDistant);

        // Commentaire a rajouter si vous voulez une execution manuelle.
        terminalClient();

        socketClient1 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
        if (socketClient1 < 0)
        {
            perror("Erreur lors de l'acceptation...");
            close(socketEcoute);
            exit(-4);
        }
        // Info client.
        printf("Client n°1 connecté : %s:%d\n",
               inet_ntoa(pointDeRencontreDistant.sin_addr),
               ntohs(pointDeRencontreDistant.sin_port));

        // Attente d'une deuxieme connection.
        printf("En attente de la connexion du joueur 2...\n");

        // Commentaire a rajouter si vous voulez une execution manuelle.
        terminalClient();
        longueurAdresse = sizeof(pointDeRencontreDistant);
        socketClient2 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
        if (socketClient2 < 0)
        {
            perror("Erreur lors de l'acceptation...");
            close(socketEcoute);
            exit(-4);
        }

        // Info client.
        printf("Client n°2 connecté : %s:%d\n",
               inet_ntoa(pointDeRencontreDistant.sin_addr),
               ntohs(pointDeRencontreDistant.sin_port));

        // Spectateur
        int listeSocketSpect[nbSpectateurMax];
        for (int nbSpectateur = 0; nbSpectateur < nbSpectateurMax; nbSpectateur++)
        {
            printf("En attente d'une connexion spectateur n°%d/%d...\n", nbSpectateur, nbSpectateurMax);
            // Commentaire a rajouter si vous voulez une execution manuelle.
            terminalClient();
            listeSocketSpect[nbSpectateur] = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);

            // Info spectateur
            printf("Spectateur connecté : %s:%d\n",
                   inet_ntoa(pointDeRencontreDistant.sin_addr),
                   ntohs(pointDeRencontreDistant.sin_port));
        }

        // Lancer le jeu serveur avec spectateur
        jeuServeur(socketClient1, socketClient2, listeSocketSpect, nbSpectateurMax);

        printf("Fin\n");
    }
    // ====== Fermeture Socket Ecoute (9) ======
    close(socketEcoute);
    printf("Socket d'écoute fermée.\n");

    return 0;
}
