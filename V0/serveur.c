#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "morpion.h"
#include "morpion.c"

#define PORT 5000 // Port sur lequel le serveur écoute

int main() {
    int server_fd, client_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    Morpion morpion;

    // Initialisation de la grille
    initialise(&morpion);

    // Création du socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Erreur : Création du socket");
        exit(EXIT_FAILURE);
    }

    // Options du socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Erreur : setsockopt");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Liaison du socket avec l'adresse et le port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erreur : bind");
        exit(EXIT_FAILURE);
    }

    // Mise en écoute des connexions entrantes
    if (listen(server_fd, 3) < 0) {
        perror("Erreur : listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexion sur le port %d...\n", PORT);

    while (1) {
        // Acceptation d'une connexion entrante
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Erreur : accept");
            exit(EXIT_FAILURE);
        }

        printf("Client connecté !\n");

        // Envoi du message "start" au client
        send(client_socket, "start", strlen("start"), 0);

        // Boucle principale pour gérer la partie
        while (1) {
            // Lecture de la case choisie par le client
            memset(buffer, 0, sizeof(buffer)); // Réinitialisation du buffer
            valread = read(client_socket, buffer, 1024);
            if (valread <= 0) {
                printf("Client déconnecté.\n");
                break;
            }

            int client_choix = atoi(buffer); // Conversion du choix du client en entier
            place(&morpion, client_choix, 'X'); // Mise à jour de la grille pour le joueur X
            printf("Le client a joué :\n");
            show(&morpion);

            // Vérification si la grille est pleine
            if (isFull(&morpion)) {
                printf("Grille pleine. Fin de la partie.\n");
                break;
            }

            // Choix aléatoire pour le serveur
            int serveur_choix;
            do {
                serveur_choix = rand() % 9 + 1; // Génération d'une case aléatoire (1-9)
            } while (morpion.grille[(serveur_choix - 1) / 3][(serveur_choix - 1) % 3] != ' ');

            place(&morpion, serveur_choix, 'O'); // Mise à jour de la grille pour le joueur O
            printf("Le serveur a joué :\n");
            show(&morpion);

            // Envoi de la case choisie au client
            memset(buffer, 0, sizeof(buffer)); // Réinitialisation du buffer
            sprintf(buffer, "%d", serveur_choix);
            send(client_socket, buffer, strlen(buffer), 0);

            // Vérification si la grille est pleine
            if (isFull(&morpion)) {
                printf("Grille pleine. Fin de la partie.\n");
                break;
            }
        }

        // Fermeture de la connexion avec le client
        close(client_socket);
        printf("Partie terminée. En attente d'une nouvelle connexion...\n");

        // Réinitialisation de la grille pour une nouvelle partie
        initialise(&morpion);
    }

    return 0;
}
