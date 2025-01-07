#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "morpion.h"
#include "morpion.c"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        return -1;
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    Morpion morpion;
    initialise(&morpion);

    // Création du socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Erreur : Création du socket\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    // Conversion de l'adresse IP
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        printf("Adresse IP invalide : %s\n", server_ip);
        return -1;
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Erreur : Connexion au serveur (%s:%d)\n", server_ip, server_port);
        return -1;
    }

    // Attente du message "start"
    valread = read(sock, buffer, 1024);
    if (strcmp(buffer, "start") != 0) {
        printf("Erreur : Message 'start' non reçu\n");
        return -1;
    }
    printf("La partie commence !\n");
    show(&morpion);

    while (1) {
        int choix;
        printf("Choisissez une case (1-9) : ");
        scanf("%d", &choix);

        if (!isValid(choix) || morpion.grille[(choix - 1) / 3][(choix - 1) % 3] != ' ') {
            printf("Case invalide, réessayez.\n");
            continue;
        }

        // Envoi du choix au serveur
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", choix);
        send(sock, buffer, strlen(buffer), 0);

        // Mise à jour et affichage de la grille
        place(&morpion, choix, 'X');
        show(&morpion);

        // Vérifier si la grille est pleine
        if (isFull(&morpion)) {
            printf("La grille est pleine. Match nul !\n");
            break;
        }

        // Attente du choix du serveur
        memset(buffer, 0, sizeof(buffer));
        valread = read(sock, buffer, 1024);
        int serveurChoix = atoi(buffer);

        // Mise à jour de la grille avec le choix du serveur
        place(&morpion, serveurChoix, 'O');
        printf("Le serveur a joué :\n");
        show(&morpion);

        // Vérifier si la grille est pleine après le coup du serveur
        if (isFull(&morpion)) {
            printf("La grille est pleine.\n");
            break;
        }
    }

    // Fermeture de la connexion
    close(sock);
    printf("Partie terminée. Merci d'avoir joué !\n");
    return 0;
}
