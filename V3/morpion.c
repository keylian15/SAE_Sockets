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
 * isValid(&m, 2);
 * @param cell numero de la cellule
 * @return bool
 */
bool isValid(Morpion *m, int cell)
{
    if (cell < 10 && cell > 0)
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
 * place(&m, 3, x);
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
 * whoWin(&m, 1, 1); *
 * @return char
 */
char *whoWin(Morpion *m, int x, int y)
{

    if (m->grille[x][y] == 'X')
    {
        return "Client1Win";
    }
    if (m->grille[x][y] == 'O')
    {
        return "Client2Win";
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
 * Fonction gerant la logique du jeu lorsque c'est le client qui joue.
 */
void jeuClient(int socketDialogue)
{
    // Déclaration des variables.
    char messageEnvoye[LG_MESSAGE];
    char messageRecu[LG_MESSAGE];
    int bytesReceived;
    int bytesSent;
    bool conditionNb = false;

    Morpion jeu;
    initialise(&jeu);

    printf("Début du jeu.\n");
    while (1)
    {
        int choix;
        conditionNb = false;

        // Recevoir le message de tour.
        bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
        if (!verifRecu(bytesReceived, messageRecu))
        {
            return;
        }
        if (strcmp(messageRecu, "A vous de jouer") == 0)
        {
            // Montre le jeu
            show(&jeu);

            // Choix de la case
            while (!conditionNb)
            {
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
                if (!verifRecu(bytesReceived, messageRecu))
                {
                    return;
                }
                if (strcmp(messageRecu, "confirm") == 0)
                {
                    conditionNb = true;
                }
            }

            // Placer la case du client coté client.
            place(&jeu, choix, 'X');
            show(&jeu);

            // Recevoir le Client1Win ou Clien1End ou Rien.
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if (!verifRecu(bytesReceived, messageRecu))
            {
                return;
            }
            if (strcmp(messageRecu, "Client1Win") == 0)
            {
                printf("Le client 1 a gagné !\n");
                close(socketDialogue);
                printf("Socket de dialogue fermée.\n");
                break;
            }
            if (strcmp(messageRecu, "Client1End") == 0)
            {
                printf("Match nul !\n");
                close(socketDialogue);
                printf("Socket de dialogue fermée.\n");
                break;
            }

            // Recoit instruction inversion tours
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if (!verifRecu(bytesReceived, messageRecu))
            {
                return;
            }

            // Recevoir la case du client 2.
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if (!verifRecu(bytesReceived, messageRecu))
            {
                return;
            }

            // Placer la case du client 2 coté client.
            int case_client_2 = atoi(messageRecu);
            place(&jeu, case_client_2, 'O');

            // Recevoir le Client2Win ou Clien2End ou Rien.
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if (!verifRecu(bytesReceived, messageRecu))
            {
                return;
            }
            if (strcmp(messageRecu, "Client2Win") == 0)
            {
                printf("Le client 2 a gagné !\n");
                close(socketDialogue);
                printf("Socket de dialogue fermée.\n");
                break;
            }
            if (strcmp(messageRecu, "Client2End") == 0)
            {
                printf("Match nul !\n");
                close(socketDialogue);
                printf("Socket de dialogue fermée.\n");
                break;
            }
        }

        else
        {
            memset(messageRecu, 0x00, LG_MESSAGE);

            // Reception la case du client 1.
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if (!verifRecu(bytesReceived, messageRecu))
            {
                return;
            }

            // Placer la case du client 1 coté client.
            int case_client_1 = atoi(messageRecu);
            place(&jeu, case_client_1, 'X');
            show(&jeu);

            // Recevoir le Client1Win ou Clien1End ou Rien.
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if (!verifRecu(bytesReceived, messageRecu))
            {
                return;
            }
            if (strcmp(messageRecu, "Client1Win") == 0)
            {
                printf("Le client 1 a gagné !\n");
                close(socketDialogue);
                printf("Socket de dialogue fermée.\n");
                break;
            }
            if (strcmp(messageRecu, "Client1End") == 0)
            {
                printf("Match nul !\n");
                close(socketDialogue);
                printf("Socket de dialogue fermée.\n");
                break;
            }

            // Reception instruction tour
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if (!verifRecu(bytesReceived, messageRecu))
            {
                return;
            }

            // show(&jeu);

            // Choix de la case
            while (!conditionNb)
            {
                printf("Choisissez votre case (1 à 9) :\n");
                scanf("%d", &choix);

                // Envoi le choix au serveur.
                sleep(1);
                snprintf(messageEnvoye, sizeof(messageEnvoye), "%d", choix);
                bytesSent = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);

                // ====== Réception Message Serveur ======
                bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
                if (!verifRecu(bytesReceived, messageRecu))
                {
                    return;
                }
                if (strcmp(messageRecu, "confirm") == 0)
                {
                    conditionNb = true;
                }
            }

            // Placer la case du client 2 coté client.
            place(&jeu, choix, 'O');
            show(&jeu);

            // Recevoir le Client2Win ou Clien2End ou Rien.
            bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
            if (!verifRecu(bytesReceived, messageRecu))
            {
                return;
            }
            if (strcmp(messageRecu, "Client2Win") == 0)
            {
                printf("Le client 2 a gagné !\n");
                close(socketDialogue);
                printf("Socket de dialogue fermée.\n");
                break;
            }
            if (strcmp(messageRecu, "Client2End") == 0)
            {
                printf("Match nul !\n");
                close(socketDialogue);
                printf("Socket de dialogue fermée.\n");
                break;
            }
        }
    }
}

/**
 * Fonction gerant la logique du jeu lorsque c'est le serveur qui joue.
 */
void jeuServeur(int socketClient1, int socketClient2, int listeSocketSpect[], int nbSpectateur)
{
    printf("Début du jeu.\n");
    int bytesSent;
    int bytesReceived;
    char messageRecu[LG_MESSAGE];
    char messageEnvoye[LG_MESSAGE];
    bool conditionNb = false;
    int case_client;
    char *reponse;

    Morpion jeu;
    initialise(&jeu);

    while (1)
    {
        // ====== Envoi Message de départ ======
        // Au client 1 :
        sleep(1);
        strcpy(messageEnvoye, "start");
        bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
        verifEnvoye(bytesSent, messageEnvoye);

        // Au client 2 :
        sleep(1);
        strcpy(messageEnvoye, "start");
        bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
        verifEnvoye(bytesSent, messageEnvoye);

        // Au Spectateur :
        sleep(1);
        strcpy(messageEnvoye, "startspectateur");
        sendToSpectate(listeSocketSpect, nbSpectateur, messageEnvoye);
        // ====== Boucle de jeu ======
        while (1)
        {
            conditionNb = false;
            // Envoie message de tour.
            // Au client 1 :
            sleep(1);
            strcpy(messageEnvoye, "A vous de jouer");
            bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // Au client 2 :
            sleep(1);
            strcpy(messageEnvoye, "Le joueur 1 joue");
            bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // Recevoir case client 1
            while (!conditionNb)
            {
                memset(messageRecu, 0x00, LG_MESSAGE);
                bytesReceived = recv(socketClient1, messageRecu, LG_MESSAGE, 0);
                if (!verifRecu(bytesReceived, messageRecu))
                {
                    return;
                }

                case_client = atoi(messageRecu);
                if (!isValid(&jeu, case_client))
                {
                    sleep(1);
                    strcpy(messageEnvoye, "erreur");
                    bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                    verifEnvoye(bytesSent, messageEnvoye);
                }
                else
                {
                    conditionNb = true;
                }
            }

            // Valider la case du client 1
            sleep(1);
            strcpy(messageEnvoye, "confirm");
            bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // Envoyer la case au client 2.
            sleep(1);
            snprintf(messageEnvoye, LG_MESSAGE, "%d", case_client);
            bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // Envoyer la case au spectateur
            sleep(1);
            snprintf(messageEnvoye, LG_MESSAGE, "%d", case_client);
            sendToSpectate(listeSocketSpect, nbSpectateur, messageEnvoye);

            // Placer la case du client 1 coté serveur.
            place(&jeu, case_client, 'X');
            show(&jeu);

            reponse = checkWin(&jeu);
            if (strcmp(reponse, "Client1Win") == 0)
            {
                // Envoie Client 1 :
                sleep(1);
                strcpy(messageEnvoye, "Client1Win");
                bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie Client 2 :
                sleep(1);
                strcpy(messageEnvoye, "Client1Win");
                bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie au spectateur :
                sleep(1);
                strcpy(messageEnvoye, "Client1Win");
                sendToSpectate(listeSocketSpect, nbSpectateur, messageEnvoye);

                close(socketClient1);
                close(socketClient2);
                close(listeSocketSpect);
                printf("Socket de dialogue fermée.\n");
                return; // Attendre un autre client après la partie terminée
            }
            if (isFull(&jeu))
            {
                // Envoie Client 1 :
                sleep(1);
                strcpy(messageEnvoye, "Client1End");
                bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie Client 2 :
                sleep(1);
                strcpy(messageEnvoye, "Client1End");
                bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie Spectateur :
                sleep(1);
                strcpy(messageEnvoye, "Client1End");
                sendToSpectate(listeSocketSpect, nbSpectateur, messageEnvoye);

                close(socketClient1);
                close(socketClient2);
                close(listeSocketSpect);
                printf("Socket de dialogue fermée.\n");
                return; // Attendre un autre client après la partie terminée
            }
            else
            {
                // Envoie Client 1 :
                sleep(1);
                strcpy(messageEnvoye, "rien");
                bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie Client 2 :
                sleep(1);
                strcpy(messageEnvoye, "rien");
                bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
            }

            // Envoie instruction tour.
            // Au client 1 :
            sleep(1);
            strcpy(messageEnvoye, "Le joueur 2 joue");
            bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // Au client 2 :
            sleep(1);
            strcpy(messageEnvoye, "A vous de jouer");
            bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // Recevoir case client 2
            conditionNb = false;
            while (!conditionNb)
            {
                memset(messageRecu, 0x00, LG_MESSAGE);
                bytesReceived = recv(socketClient2, messageRecu, LG_MESSAGE, 0);
                if (!verifRecu(bytesReceived, messageRecu))
                {
                    return;
                }

                case_client = atoi(messageRecu);
                if (!isValid(&jeu, case_client))
                {
                    sleep(1);
                    strcpy(messageEnvoye, "erreur");
                    bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                    verifEnvoye(bytesSent, messageEnvoye);
                }
                else
                {
                    conditionNb = true;
                }
            }

            // Confirmer la case du joueur 2.
            sleep(1);
            strcpy(messageEnvoye, "confirm");
            bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // Envoyer la case au client 1.
            sleep(1);
            snprintf(messageEnvoye, LG_MESSAGE, "%d", case_client);
            bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
            verifEnvoye(bytesSent, messageEnvoye);

            // Placer la case du client 2 coté serveur.
            place(&jeu, case_client, 'O');
            show(&jeu);

            // Placer la case du client coté spectateur
            sleep(1);
            snprintf(messageEnvoye, LG_MESSAGE, "%d", case_client);
            sendToSpectate(listeSocketSpect, nbSpectateur, messageEnvoye);

            reponse = checkWin(&jeu);
            if (strcmp(reponse, "Client2Win") == 0)
            {
                // Envoie Client 1 :
                sleep(1);
                strcpy(messageEnvoye, "Client2Win");
                bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie Client 2 :
                sleep(1);
                strcpy(messageEnvoye, "Client2Win");
                bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie Spectateur
                sleep(1);
                strcpy(messageEnvoye, "Client2Win");
                sendToSpectate(listeSocketSpect, nbSpectateur, messageEnvoye);

                close(socketClient1);
                close(socketClient2);
                close(listeSocketSpect);
                printf("Socket de dialogue fermée.\n");
                return; // Attendre un autre client après la partie terminée
            }
            if (isFull(&jeu))
            {
                // Envoie Client 1 :
                sleep(1);
                strcpy(messageEnvoye, "Client2End");
                bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie Client 2 :
                sleep(1);
                strcpy(messageEnvoye, "Client2End");
                bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie Spectateur
                sleep(1);
                strcpy(messageEnvoye, "Client2End");
                sendToSpectate(listeSocketSpect, nbSpectateur, messageEnvoye);

                close(socketClient1);
                close(socketClient2);
                close(listeSocketSpect);
                printf("Socket de dialogue fermée.\n");
                return; // Attendre un autre client après la partie terminée
            }
            else
            {
                // Envoie Client 1 :
                sleep(1);
                strcpy(messageEnvoye, "rien");
                bytesSent = send(socketClient1, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
                // Envoie Client 2 :
                sleep(1);
                strcpy(messageEnvoye, "rien");
                bytesSent = send(socketClient2, messageEnvoye, strlen(messageEnvoye) + 1, 0);
                verifEnvoye(bytesSent, messageEnvoye);
            }
            // FIN DEUXIEME JOUEUR
        }
    }
}

/**
 * Fonction gerant la logique du jeu pour un spectateur
 */
void jeuSpectateur(int socketDialogue)
{
    char messageRecu[LG_MESSAGE];
    char tour = 'X'; // 'X' pour joueur 1 et 'O' pour joueur 2
    int bytesReceived;
    int case_jouee;
    Morpion jeu;

    initialise(&jeu);
    printf("Connexion au mode spectateur.\n");
    printf("En attente des mises à jour du jeu...\n");

    while (1)
    {

        memset(messageRecu, 0x00, LG_MESSAGE);

        bytesReceived = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
        if (!verifRecu(bytesReceived, messageRecu))
        {
            return;
        }

        if (strcmp(messageRecu, "Client1Win") == 0)
        {
            printf("Le joueur 1 a gagné !\n");
            break;
        }
        else if (strcmp(messageRecu, "Client2Win") == 0)
        {
            printf("Le joueur 2 a gagné !\n");
            break;
        }
        else if (strcmp(messageRecu, "Client1End") == 0 || strcmp(messageRecu, "Client2End") == 0)
        {
            printf("Match nul !\n");
            break;
        }
        else
        {
            case_jouee = atoi(messageRecu);

            // Mettre à jour la grille avec le symbole du joueur actuel
            place(&jeu, case_jouee, tour);

            // Alterner le joueur
            if (tour == 'X')
            {
                tour = 'O';
            }
            else
            {
                tour = 'X';
            }

            // Afficher la grille mise à jour
            show(&jeu);
        }
    }

    // Fermeture de la socket une fois terminé
    close(socketDialogue);
    printf("Mode spectateur terminé.\n Socket fermée.\n");
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

void sendToSpectate(int sockets[], int nbSpectateurs, const char *message)
{
    int bytesSent;
    for (int i = 0; i < nbSpectateurs; i++)
    {
        bytesSent = send(sockets[i], message, strlen(message), 0);
        verifEnvoye(bytesSent, message);
    }
}
