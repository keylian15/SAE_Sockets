#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "morpion.h"  // Inclure le fichier d'en-tête pour utiliser les fonctions du jeu
#include "morpion.c"
#define LG_MESSAGE 256

int main(int argc, char *argv[]) {
    int descripteurSocket;
    struct sockaddr_in sockaddrLocal, sockaddrDistant;
    socklen_t longueurAdresse;
    char messageRecu[LG_MESSAGE];
    char messageEnvoye[LG_MESSAGE];
    int nb;
    char ip_dest[16];
    int port_dest;

    if (argc > 1) {
        strncpy(ip_dest, argv[1], 16);
        sscanf(argv[2], "%d", &port_dest);
    } else {
        printf("USAGE : %s ip port\n", argv[0]);
        exit(-1);
    }

    // Création socket
    descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (descripteurSocket < 0) {
        perror("Erreur en création de la socket...");
        exit(-1);
    }

    memset(&sockaddrLocal, 0x00, sizeof(sockaddrLocal));
    sockaddrLocal.sin_family = AF_INET;
    sockaddrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddrLocal.sin_port = 0;

    if (bind(descripteurSocket, (struct sockaddr *)&sockaddrLocal, sizeof(sockaddrLocal)) < 0) {
        perror("Erreur d'attachement de l'adresse locale...");
        close(descripteurSocket);
        exit(-2);
    }

    memset(&sockaddrDistant, 0x00, sizeof(sockaddrDistant));
    sockaddrDistant.sin_family = AF_INET;
    sockaddrDistant.sin_port = htons(port_dest);
    inet_aton(ip_dest, &sockaddrDistant.sin_addr);

    if (connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant, sizeof(sockaddrDistant)) < 0) {
        perror("Erreur de connexion...");
        close(descripteurSocket);
        exit(-3);
    }

    // Attendre le message de départ
    nb = recv(descripteurSocket, messageRecu, LG_MESSAGE, 0);
    if (nb <= 0) {
        perror("Erreur lors de la réception...");
        close(descripteurSocket);
        exit(-4);
    }
    printf("Message du serveur : %s\n", messageRecu);

    while (1) {
        // Demander au client de choisir une case
        printf("Choisissez une case (1 à 9) : ");
        int choix;
        scanf("%d", &choix);

        if (!isValid(choix)) {
            printf("Choix invalide !\n");
            continue;
        }

        snprintf(messageEnvoye, sizeof(messageEnvoye), "%d", choix);
        send(descripteurSocket, messageEnvoye, strlen(messageEnvoye) + 1, 0);

        // Recevoir la mise à jour
        nb = recv(descripteurSocket, messageRecu, LG_MESSAGE, 0);
        if (nb <= 0) {
            perror("Erreur lors de la réception...");
            close(descripteurSocket);
            exit(-5);
        }
        printf("Mise à jour du serveur : %s\n", messageRecu);
    }

    close(descripteurSocket);
    return 0;
}
