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
#include <time.h>
#define PORT 5000
#define LG_MESSAGE 256
/**
 * Vérifie si la grille est entièrement remplie.
 * isFull(&m);
 * @param m Pointeur vers la structure Morpion.
 * @return true si la grille est pleine, false sinon.
 */

bool isFull(const Morpion *m)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (m->grille[i][j] == ' ')
            {
                return false;
            }
        }
    }
    return true;
}

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
    printf("\n");
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
    printf("\n");
}

/**
 * verifie si la cellule existe
 * isValid(2);
 * @param cell numero de la cellule
 * @return bool
 */
bool isValid(Morpion *m, int cell)
{
    if (cell < 10 || cell > 0)
    {
        if (m->grille[(cell - 1) / 3][(cell - 1) % 3] == ' ')
        {
            return true;
        }
        return false;
    }
    return false;
}

/**
 * place dans la grille la forme demander
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

/**
 * verifie qui a gagner a l'aide d'une combinaison de coordonée
 * whoWin(&m,1,1); *
 * @return char
 */
char *whoWin(Morpion *m, int x, int y)
{

    if (m->grille[x][y] == 'X')
    {
        return "Xwin";
    }
    if (m->grille[x][y] == 'O')
    {
        return "Owin";
    }
    else
    {
        return "continue";
    }
}

/**
 * verifie si il y a un gagnant dans la grille et renvoie le gagnant si oui
 * checkwin(&m);
 * @return char
 */
char *checkWin(Morpion *m)
{
    for (int i = 0; i < 3; i++)
    {
        if (m->grille[i][0] != ' ' && m->grille[i][1] == m->grille[i][0] && m->grille[i][2] == m->grille[i][0])
        {
            return whoWin(m, i, 1);
        }
    }
    for (int j = 0; j < 3; j++)
    {
        if (m->grille[0][j] != ' ' && m->grille[0][j] == m->grille[1][j] && m->grille[2][j] == m->grille[0][j])
        {
            return whoWin(m, 1, j);
        }
    }
    if (m->grille[0][0] != ' ' && m->grille[0][0] == m->grille[1][1] && m->grille[2][2] == m->grille[1][1])
    {
        return whoWin(m, 1, 1);
    }
    else if (m->grille[0][2] != ' ' && m->grille[0][2] == m->grille[1][1] && m->grille[2][0] == m->grille[1][1])
    {
        return whoWin(m, 1, 1);
    }
    else
    {
        return "continue";
    }
}

/**
 * Fonction permettant d'avoir la liste des cases vides de la grille.
 * getCaseVide(&m);
 * @param *m Le morpion
 * @return La liste sous forme : "2 4" par exemple. */
char *getCaseVide(Morpion *m)
{
    static char listeCase[20];
    int index = 0;
    int count = 1;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (m->grille[i][j] == ' ')
            {
                if (index > 0)
                {
                    listeCase[index++] = '-';
                }
                listeCase[index++] = '0' + count;
            }
            count++;
        }
    }

    listeCase[index] = '\0';
    return listeCase;
}

/**
 * Fonction gerant la logique du jeu lorsque c'est le client qui joue.
 */
void jeuClient(int socketDialogue)
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
            printf("Choisissez votre case (1 à 9) :\n");
            scanf("%d", &choix);

            // Envoi le choix au serveur.
            sleep(1);
            snprintf(messageEnvoye, sizeof(messageEnvoye), "%d", choix);
            bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // ====== Réception Message Serveur ======
            memset(messageRecu, 0x00, LG_MESSAGE);
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if(!verifRecu(bytesReceived, messageRecu)){
                return;
            }

            if (strcmp(messageRecu, "confirm") == 0)
            {
                condition = true;
            }
        }

        // Placer la case du client coté client.
        place(&jeu, choix, 'X');
        show(&jeu);

        // Recevoir le Xwin ou Xend ou Rien.
        bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
        if(!verifRecu(bytesReceived, messageRecu)){
                return;
            }

        if (strcmp(messageRecu, "Xwin") == 0)
        {
            printf("Le client a gagné !\n");
            close(socketDialogue);
            printf("Socket de dialogue fermée.\n");
            break;
        }
        if (strcmp(messageRecu, "Xend") == 0)
        {
            printf("Match nul !\n");
            close(socketDialogue);
            printf("Socket de dialogue fermée.\n");
            break;
        }

        // Recevoir la case serveur.
        bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
        if(!verifRecu(bytesReceived, messageRecu)){
                return;
            }

        // Place la case du serveur coté client.
        printf("Au serveur de jouer : \n");
        int case_serveur = atoi(messageRecu);
        place(&jeu, case_serveur, 'O');
        show(&jeu);

        // Recevoir le message du serveur.
        bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
        if(!verifRecu(bytesReceived, messageRecu)){
                return;
            }

        if (strcmp(messageRecu, "Owin") == 0)
        {
            printf("Le serveur a gagné !\n");
            close(socketDialogue);
            printf("Socket de dialogue fermée.\n");
            break;
        }
        if (strcmp(messageRecu, "Oend") == 0)
        {
            printf("Match nul !\n");
            close(socketDialogue);
            printf("Socket de dialogue fermée.\n");
            break;
        }
    }
}

/**
 * Fonction gerant la logique du jeu lorsque c'est le serveur qui joue.
 */
void jeuServeur(int socketDialogue)
{
    printf("Début du jeu.\n");
    int bytesSent;
    int bytesReceived;
    char messageRecu[LG_MESSAGE];
    char messageEnvoye[LG_MESSAGE];

    Morpion jeu;
    initialise(&jeu);

    while (1)
    {
        // ====== Envoi Message de départ ======
        sleep(1);
        strcpy(messageEnvoye, "start");
        bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
        verifEnvoye(bytesSent, messageEnvoye);

        // ====== Boucle de jeu ======
        while (1)
        {
            memset(messageRecu, 0x00, LG_MESSAGE);
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if(!verifRecu(bytesReceived, messageRecu)){
                return;
            }

            int case_client = atoi(messageRecu);

            if (!isValid(&jeu, case_client))
            {
                sleep(1);
                strcpy(messageEnvoye, "erreur");
                bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
            }
            else
            {
                sleep(1);
                strcpy(messageEnvoye, "confirm");
                bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);

                // Placer la case du client coté serveur.
                place(&jeu, case_client, 'X');
                show(&jeu);

                char *reponse = checkWin(&jeu);
                if (strcmp(reponse, "Xwin") == 0)
                {
                    sleep(1);
                    strcpy(messageEnvoye, "Xwin");
                    bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                    verifEnvoye(bytesSent, messageEnvoye);
                    close(socketDialogue);
                    printf("Socket de dialogue fermée.\n");
                    return; // Attendre un autre client après la partie terminée
                }
                if (isFull(&jeu))
                {
                    sleep(1);
                    strcpy(messageEnvoye, "Xend");
                    bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                    verifEnvoye(bytesSent, messageEnvoye);
                    close(socketDialogue);
                    printf("Socket de dialogue fermée.\n");
                    return; // Attendre un autre client après la partie terminée
                }
                else
                {
                    sleep(1);
                    strcpy(messageEnvoye, "rien");
                    bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                    verifEnvoye(bytesSent, messageEnvoye);
                }

                char *listeCaseVide = getCaseVide(&jeu);
                char *token = strtok(listeCaseVide, "-");
                char *tokens[50];
                int count = 0;

                while (token != NULL)
                {
                    tokens[count++] = token;
                    token = strtok(NULL, "-");
                }

                srand(time(NULL));
                int index_case_serveur = rand() % count;
                int case_serveur = atoi(tokens[index_case_serveur]);

                sleep(1);
                snprintf(messageEnvoye, LG_MESSAGE, "%d", case_serveur);
                bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);

                place(&jeu, case_serveur, 'O');
                show(&jeu);

                reponse = checkWin(&jeu);
                if (strcmp(reponse, "Owin") == 0)
                {
                    sleep(1);
                    strcpy(messageEnvoye, "Owin");
                    bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                    verifEnvoye(bytesSent, messageEnvoye);
                    close(socketDialogue);
                    printf("Socket de dialogue fermée.\n");
                    return; // Attendre un autre client après la partie terminée
                }
                if (isFull(&jeu))
                {
                    sleep(1);
                    strcpy(messageEnvoye, "Oend");
                    bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                    verifEnvoye(bytesSent, messageEnvoye);
                    close(socketDialogue);
                    printf("Socket de dialogue fermée.\n");
                    return; // Attendre un autre client après la partie terminée
                }
                else
                {
                    sleep(1);
                    strcpy(messageEnvoye, "continue");
                    bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                    verifEnvoye(bytesSent, messageEnvoye);
                }
            }
        }
    }
}

/**
 * Fonction permettant de verifier l'envoye d'un message envoyé.
 * verifEnvoye(bytesReceived, messageEnvoye);
 * @param bytesSent le retour de la fonction send()
 * @param *messageEnvoye Le message envoyé;
 */
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

/**
 * Fonction permettant de verifier le recu d'un message receptionné.
 * if(!verifRecu(bytesReceived, messageRecu)){
                return;
            }
 * @param bytesReceived le retour de la fonction recv()
 * @param *messageRecu Le message recu;
 * @return false si erreur true si aucune erreur
 */
bool verifRecu(ssize_t bytesReceived, char *messageRecu)
{
    if (bytesReceived < 0)
    {
        perror("Erreur lors de la réception du message");
        return false;
        // Gestion d'erreur ici
    }
    else if (bytesReceived == 0)
    {
        printf("Le socket a été fermé par l'émetteur.\n");
        return false;
    }
    else
    {
        messageRecu[bytesReceived] = '\0';
        printf("Message reçu '%s' avec succès (%zd octets) \n", messageRecu, bytesReceived);
        return true;
    }
}

/**
 * Fonction permettant de lancer un autre terminal client.
 */
void terminalClient()
{
    char command_client[265];
    snprintf(command_client, sizeof(command_client), "xterm -e ./T3N_client_V1 127.0.0.1 5000 ");
    int fils2 = fork();
    if (fils2 == 0)
    {
        sleep(1);
        system(command_client);
        exit(0);
    }
}