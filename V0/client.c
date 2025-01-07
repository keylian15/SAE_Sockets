#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define PORT 5000 //(ports >= 5000 réservés pour usage explicite)
#define LG_MESSAGE 256

int main(int argc, char *argv[]) {
    int descripteurSocket;
    struct sockaddr_in sockaddrDistant;
    socklen_t longueurAdresse;
    char messageEnvoye[LG_MESSAGE]; /* message à envoyer au serveur */
    char messageRecu[LG_MESSAGE]; /* message reçu du serveur */
    int nb, lus;

    // Vérification des arguments passés en ligne de commande
    if (argc < 3) {
        printf("Usage: %s <adresse_ip> <port>\n", argv[0]);
        exit(-1);
    }
    char ip_dest[16];
    strncpy(ip_dest, argv[1], 16);
    int port_dest = atoi(argv[2]);

    // Création de la socket de communication
    descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (descripteurSocket < 0) {
        perror("Erreur en création de la socket...");
        exit(-1);
    }
    printf("Socket créée ! (%d)\n", descripteurSocket);

    // Remplissage de sockaddrDistant pour établir la connexion
    longueurAdresse = sizeof(sockaddrDistant);
    memset(&sockaddrDistant, 0x00, longueurAdresse);
    sockaddrDistant.sin_family = AF_INET;
    sockaddrDistant.sin_port = htons(port_dest);
    inet_aton(ip_dest, &sockaddrDistant.sin_addr);

    // Demande de connexion au serveur
    if (connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant, longueurAdresse) < 0) {
        perror("Erreur de connexion au serveur...");
        close(descripteurSocket);
        exit(-2);
    }
    printf("Connexion au serveur réussie !\n");

    // Envoi de la demande (heure ou date)
    printf("Entrez 'heure' ou 'date' pour la demande : ");
    fgets(messageEnvoye, LG_MESSAGE, stdin);
    messageEnvoye[strcspn(messageEnvoye, "\n")] = 0; // Supprime le '\n'

    send(descripteurSocket, messageEnvoye, strlen(messageEnvoye) + 1, 0);

    // Réception de la réponse du serveur
    lus = recv(descripteurSocket, messageRecu, LG_MESSAGE, 0);
    if (lus < 0) {
        perror("Erreur de réception...");
        close(descripteurSocket);
        exit(-3);
    }

    // Affichage de la réponse du serveur
    printf("Réponse du serveur : %s\n", messageRecu);

    // Fermeture de la socket de dialogue
    close(descripteurSocket);

    return 0;
}
