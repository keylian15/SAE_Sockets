#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>		/* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>	/* pour htons et inet_aton */

#define LG_MESSAGE 256

int main(int argc, char *argv[])
{
	int descripteurSocket;
	struct sockaddr_in sockaddrLocal;
	struct sockaddr_in sockaddrDistant;
	socklen_t longueurAdresse;

	char demande[LG_MESSAGE]; // Demande "heure" ou "date"
	char reponse[LG_MESSAGE]; // Réponse Serveur
	int nb;					  // Nb d'octets écrits et lus
	char ip_dest[16];		  // Ip destination
	int port_dest;			  // Port destination

	// ====== Gestion Erreur ======
	// ./client 127.0.0.1 5000
	if (argc > 1)
	{
		strncpy(ip_dest, argv[1], 16);
		sscanf(argv[2], "%d", &port_dest);
	}
	else
	{
		printf("USAGE : %s ip port\n", argv[0]);
		exit(-1);
	}
	// ====== Fin Gestion Erreur ======

	// ====== Création socket (1) ======
	descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (descripteurSocket < 0)
	{
		perror("Erreur en création de la socket...");
		exit(-1);
	}
	printf("Socket créée! (%d)\n", descripteurSocket);
	// ====== Fin Création socket ======

	// ====== Configuration Adresse Locale (2) ======
	memset(&sockaddrLocal, 0x00, sizeof(sockaddrLocal));
	sockaddrLocal.sin_family = AF_INET;
	sockaddrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddrLocal.sin_port = 0;
	if (bind(descripteurSocket, (struct sockaddr *)&sockaddrLocal, sizeof(sockaddrLocal)) < 0)
	{
		perror("Erreur d'attachement de l'adresse locale...");
		close(descripteurSocket);
		exit(-2);
	}
	printf("Adresse locale attachée!\n");

	// --- Configuration de l'adresse distante ---
	longueurAdresse = sizeof(sockaddrDistant);
	memset(&sockaddrDistant, 0x00, longueurAdresse);
	sockaddrDistant.sin_family = AF_INET;
	sockaddrDistant.sin_port = htons(port_dest);
	inet_aton(ip_dest, &sockaddrDistant.sin_addr);
	// ====== Fin Configuration Adresse Locale ======

	// ====== Demande Connection (3) ======
	if (connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant, longueurAdresse) < 0)
	{
		perror("Erreur de connexion avec le serveur distant...");
		close(descripteurSocket);
		exit(-3);
	}
	printf("Connexion au serveur %s:%d réussie!\n", ip_dest, port_dest);
	// ====== Fin Demande Connection ======

	// ====== Envoi Demande (4) ======
	printf("Entrez votre demande (heure/date) : ");
	scanf("%s", demande);
	nb = send(descripteurSocket, demande, strlen(demande) + 1, 0);
	if (nb <= 0)
	{
		perror("Erreur lors de l'envoi de la demande...");
		close(descripteurSocket);
		exit(-4);
	}
	printf("Demande '%s' envoyée (%d octets).\n", demande, nb);
	// ====== Fin Envoi Demande ======

	// ====== Réception Réponse (5) ======
	nb = recv(descripteurSocket, reponse, LG_MESSAGE, 0);
	if (nb <= 0)
	{
		perror("Erreur lors de la réception de la réponse...");
		close(descripteurSocket);
		exit(-5);
	}
	printf("Réponse reçue : %s\n", reponse);
	// ====== Fin Réception Réponse (5) ======

	// ====== Fermeture Socket (6) ======
	close(descripteurSocket);
	printf("Socket fermée.\n");
	// ====== Fin Fermeture Socket (6) ======

	return 0;
}
