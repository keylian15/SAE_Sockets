#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "morpion.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 5000
#define LG_MESSAGE 256

/**
 * Initialise la grille en remplissant toutes les cases avec des espaces.
 * @param m Pointeur vers la structure Morpion.
 */
void initialise(Morpion *m)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            m->grille[i][j] = ' ';
        }
    }
}

/**
 * Afficher la grille
 * show(&m);
 * @param m Pointeur vers la structure Morpion.
 */
void show(const Morpion *m)
{
    int count = 1;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (m->grille[i][j] == ' ')
            {
                printf("%d", count);
            }
            else
            {
                printf("%c", m->grille[i][j]);
            }
            if (j < 2)
                printf("|");

            count++;
        }
        printf("\n");
        if (i < 2)
            printf("-----\n");
    }
    printf("\n\n");
}

/**
 * verifie si la cellule existe
 * isValid(2);
 * @param cell numero de la cellule
 * @return bool
 */
bool isValid(int cell)
{
    if (cell > 9 || cell < 1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * place dans la grille la forme demannder
 * place(&m,3,x);
 * @param cell numero de la cellule
 * @param form la forme qui est entrée
 */
void place(Morpion *m, int cell, char form)
{
    int count = 1;
    for (int cellx = 0; cellx < 3; cellx++)
    {
        for (int celly = 0; celly < 3; celly++)
        {
            if (count == cell)
            {
                m->grille[cellx][celly] = form;
                return;
            }
            count++;
        }
    }
}

void jeuClient(int descripteurSocket)
{
    // Déclaration des variables.
    char messageEnvoye[LG_MESSAGE];
    char messageRecu[LG_MESSAGE];
    int bytesReceived;
    int bytesSent;
    bool condition = false;

    Morpion jeu;
    initialise(&jeu);

    printf("Début du jeu.\n");
    while (1)
    {
        // Montre la grille
        show(&jeu);
        int choix;
        condition = false;
        while (!condition)
        {
            // Demander au client de choisir une case
            printf("Choisissez votre case (1 à 9) :\n ");
            scanf("%d", &choix);

            // Envoi le choix au serveur.
            snprintf(messageEnvoye, sizeof(messageEnvoye), "%d", choix);
            bytesSent = send(descripteurSocket, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // ====== Réception Message Serveur ======
            memset(messageRecu, 0x00, LG_MESSAGE);
            bytesReceived = recv(descripteurSocket, messageRecu, LG_MESSAGE, 0);
            verifRecu(bytesReceived, messageRecu);

            if (strcmp(messageRecu, "confirm") == 0)
            {
                condition = true;
            }
        }

        // Placer la case du client coté client.
        place(&jeu, choix, 'X');
        show(&jeu);

        // Recevoir la mise à jour
        bytesReceived = recv(descripteurSocket, messageRecu, LG_MESSAGE, 0);
        verifRecu(bytesReceived, messageRecu);

        // Place la case du serveur coté client.
        printf("Au serveur de jouer : \n");
        int case_serveur = atoi(messageRecu);
        place(&jeu, case_serveur, 'O');
    }
}

void jeuServeur(int socketDialogue)
{
    printf("Début du jeu.\n");
    // Déclaration des variables.
    int bytesSent;
    int bytesReceived;
    char messageRecu[LG_MESSAGE];
    char messageEnvoye[LG_MESSAGE];

    Morpion jeu;
    initialise(&jeu);

    // ====== Envoi Message de départ ======
    strcpy(messageEnvoye, "start");
    bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
    verifEnvoye(bytesSent, messageEnvoye);
    // ====== Boucle de jeu ======
    while (1)
    {
        // ====== Réception Message Client ======
        memset(messageRecu, 0x00, LG_MESSAGE);
        bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
        verifRecu(bytesReceived, messageRecu);

        // Conversion de la case choisie en int
        int case_client = atoi(messageRecu);

        // Verification choix.
        if (!isValid(case_client))
        {
            strcpy(messageEnvoye, "erreur");
            bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);
        }
        else
        {

            strcpy(messageEnvoye, "confirm");
            bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // Placer la case du client coté serveur.
            place(&jeu, case_client, 'X');
            show(&jeu);

            // Le serveur joue en choisissant une case au hasard
            int case_serveur = rand() % 9 + 1;

            // Envoie de la case serveur.
            messageEnvoye[LG_MESSAGE];
            snprintf(messageEnvoye, LG_MESSAGE, "%d", case_serveur);
            bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);
            
            place(&jeu, case_serveur, 'O');
            show(&jeu);
        }
    }

    // ====== Fermeture Socket Dialogue (8) ======
    close(socketDialogue);
    printf("Socket de dialogue fermée.\n");
}

void verifEnvoye(ssize_t bytesSent, const char *messageEnvoye)
{
    if (bytesSent < 0)
    {
        perror("Erreur lors de l'envoi du message");
        // Gestion d'erreur ici (ex: fermer socket, quitter, etc.)
    }
    else if ((size_t)bytesSent != strlen(messageEnvoye) + 1)
    {
        fprintf(stderr, "Tous les octets n'ont pas été envoyés (%zd/%zd)\n",
                bytesSent, strlen(messageEnvoye) + 1);
        // Gestion pour les envois partiels (ré-envoi possible ici)
    }
    else
    {
        printf("Message envoyé '%s' avec succès (%zd octets)\n", messageEnvoye, bytesSent);
    }
}

void verifRecu(ssize_t bytesReceived, char *messageRecu)
{
    if (bytesReceived < 0)
    {
        perror("Erreur lors de la réception du message");
        // Gestion d'erreur ici (ex: fermer socket, quitter, etc.)
    }
    else if (bytesReceived == 0)
    {
        printf("Le socket a été fermé par l'émetteur.\n");
        // Fin de connexion propre, gestion ici si nécessaire
    }
    else
    {
        messageRecu[bytesReceived] = '\0'; // Terminer correctement la chaîne reçue
        printf("Message reçu '%s' avec succès (%zd octets) \n", messageRecu ,bytesReceived);
    }
}
