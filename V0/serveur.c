#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>		/* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>	/* pour htons et inet_aton */
#include <time.h> /* pour lire l'heure et la date */
#include <stdlib.h>

#define PORT 5000 // (ports >= 5000 réservés pour usage explicite)
#define LG_MESSAGE 256

void lire_heure(char* heure) {
    FILE *fpipe;
    fpipe = popen("date '+%X'", "r");
    if (fpipe == NULL) {
        perror("popen");
        exit(-1);
    }
    fgets(heure, LG_MESSAGE, fpipe);
    pclose(fpipe);
}

void lire_date(char* date) {
    FILE *fpipe;
    fpipe = popen("date '+%A %d %B %Y'", "r");
    if (fpipe == NULL) {
        perror("popen");
        exit(-1);
    }
    fgets(date, LG_MESSAGE, fpipe);
    pclose(fpipe);
}

int main(int argc, char *argv[]) {
    int descripteurSocket;
    struct sockaddr_in sockaddrDistant;
    socklen_t longueurAdresse;
    char messageRecu[LG_MESSAGE]; /* message reçu du client */
    int nb, lus;
    char buffer[LG_MESSAGE]; /* message à envoyer au client */

    // Création de la socket d'écoute
    descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (descripteurSocket < 0) {
        perror("Erreur en création de la socket...");
        exit(-1);
    }
    printf("Socket créée ! (%d)\n", descripteurSocket);

    // Remplissage de sockaddrDistant pour attacher l'adresse locale
    longueurAdresse = sizeof(sockaddrDistant);
    memset(&sockaddrDistant, 0x00, longueurAdresse);
    sockaddrDistant.sin_family = AF_INET;
    sockaddrDistant.sin_port = htons(PORT);
    sockaddrDistant.sin_addr.s_addr = htonl(INADDR_ANY);

    // Attachement de l'adresse locale à la socket
    if (bind(descripteurSocket, (struct sockaddr *)&sockaddrDistant, longueurAdresse) < 0) {
        perror("Erreur d'attachement de l'adresse locale...");
        close(descripteurSocket);
        exit(-2);
    }
    printf("Adresse locale attachée !\n");

    // Déclaration du nombre maximum de connexions
    if (listen(descripteurSocket, 5) < 0) {
        perror("Erreur de mise en écoute...");
        close(descripteurSocket);
        exit(-3);
    }
    printf("Serveur en écoute sur le port %d...\n", PORT);

    // Attente de la demande de connexion du client
    while (1) {
        int socketDialogue = accept(descripteurSocket, NULL, NULL);
        if (socketDialogue < 0) {
            perror("Erreur lors de l'acceptation de la connexion...");
            close(descripteurSocket);
            exit(-4);
        }

        // Réception de la demande du client
        memset(messageRecu, 0, LG_MESSAGE);
        lus = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
        if (lus < 0) {
            perror("Erreur de réception...");
            close(socketDialogue);
            continue;
        }

        // Traitement de la demande (heure ou date)
        if (strcmp(messageRecu, "heure") == 0) {
            char heure[LG_MESSAGE];
            lire_heure(heure);
            snprintf(buffer, LG_MESSAGE, "Heure actuelle : %s", heure);
        } else if (strcmp(messageRecu, "date") == 0) {
            char date[LG_MESSAGE];
            lire_date(date);
            snprintf(buffer, LG_MESSAGE, "Date actuelle : %s", date);
        } else {
            snprintf(buffer, LG_MESSAGE, "Commande inconnue");
        }

        // Envoi de la réponse au client
        send(socketDialogue, buffer, strlen(buffer) + 1, 0);

        // Fermeture de la socket de dialogue
        close(socketDialogue);
    }

    // Fermeture de la socket d'écoute avant de quitter
    close(descripteurSocket);
    return 0;
}
