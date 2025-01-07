#include <stdbool.h>
#include <stdio.h>
#include "morpion.h"
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
            if (count==cell)
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
    int nb;
    bool condition = false;

    Morpion jeu;
    initialise(&jeu);

    while (1)
    {
        // Montre la grille
        show(&jeu);
        int choix;

        while (!condition)
        {
            // Demander au client de choisir une case
            printf("Choisissez votre case (1 à 9) : ");
            scanf("%d", &choix);

            // Envoi le choix au serveur.
            snprintf(messageEnvoye, sizeof(messageEnvoye), "%d", choix);
            send(descripteurSocket, messageEnvoye, strlen(messageEnvoye) + 1, 0);

            // ====== Réception Message Serveur ======
            memset(messageRecu, 0x00, LG_MESSAGE);
            nb = recv(descripteurSocket, messageRecu, LG_MESSAGE, 0);
            if (nb <= 0)
            {
                perror("Erreur lors de la réception...");
                close(descripteurSocket);
                continue;
            }
            else if (strcmp(messageRecu, "confirm") == 0)
            {
                condition = true;
            }
        }

        // Placer la case du client coté client.
        place(&jeu, choix, 'X');
        show(&jeu);

        // Recevoir la mise à jour
        nb = recv(descripteurSocket, messageRecu, LG_MESSAGE, 0);
        if (nb <= 0)
        {
            perror("Erreur lors de la réception...");
            close(descripteurSocket);
            exit(-5);
        }

        // Place la case du serveur coté client.
        int case_serveur = atoi(messageRecu);
        place(&jeu, case_serveur, 'O');

        show(&jeu);
    }
}


void jeuServeur(int socketDialogue)
{
    printf("Début du jeu");
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
        printf("Case client joué %d", case_client);

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