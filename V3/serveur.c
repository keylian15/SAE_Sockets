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
    int socketEcoute, socketClient1, socketClient2, socketSpectateur;
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
        printf("En attente d'une deuxieme connexion...\n");
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

        // spectateur
        printf("En attente d'une connexion spectateur (optionnelle)...\n");
        socketSpectateur = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);

        printf("Spectateur connecté : %s:%d\n",
               inet_ntoa(pointDeRencontreDistant.sin_addr),
               ntohs(pointDeRencontreDistant.sin_port));

        // Lancer le jeu serveur avec spectateur
        jeuServeur(socketClient1, socketClient2, socketSpectateur);

        printf("Fin\n");
    }
    // ====== Fermeture Socket Ecoute (9) ======
    close(socketEcoute);
    printf("Socket d'écoute fermée.\n");

    return 0;
}
