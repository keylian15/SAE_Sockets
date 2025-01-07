#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>		/* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>	/* pour htons et inet_aton */

#define PORT 5000
#define LG_MESSAGE 256

void lire_heure(char *heure)
{
	FILE *fpipe = popen("date '+%X'", "r");
	if (fpipe == NULL)
	{
		perror("Erreur lors de l'exécution de la commande date");
		exit(-1);
	}
	fgets(heure, LG_MESSAGE, fpipe);
	pclose(fpipe);
}

void lire_date(char *date)
{
	FILE *fpipe = popen("date '+%A %d %B %Y'", "r");
	if (fpipe == NULL)
	{
		perror("Erreur lors de l'exécution de la commande date");
		exit(-1);
	}
	fgets(date, LG_MESSAGE, fpipe);
	pclose(fpipe);
}

int main()
{
	int socketEcoute, socketDialogue;
	struct sockaddr_in pointDeRencontreLocal, pointDeRencontreDistant;
	socklen_t longueurAdresse;

	char messageRecu[LG_MESSAGE];
	char messageEnvoye[LG_MESSAGE];
	int nb;

	// ====== Création socket (1) ======
	socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
	if (socketEcoute < 0)
	{
		perror("Erreur en création de la socket...");
		exit(-1);
	}
	printf("Socket d'écoute créée! (%d)\n", socketEcoute);

	// ====== Configuration Attachement Local (2) ======
	memset(&pointDeRencontreLocal, 0x00, sizeof(pointDeRencontreLocal));
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY);
	pointDeRencontreLocal.sin_port = htons(PORT);
	if (bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, sizeof(pointDeRencontreLocal)) < 0)
	{
		perror("Erreur d'attachement de l'adresse locale...");
		close(socketEcoute);
		exit(-2);
	}
	printf("Adresse locale attachée.\n");

	// ====== Déclaration Nombre Maximum Connexions (3) ======
	if (listen(socketEcoute, 5) < 0)
	{
		perror("Erreur lors de la mise en écoute...");
		close(socketEcoute);
		exit(-3);
	}
	printf("En écoute sur le port %d...\n", PORT);

	while (1)
	{
		// ====== Attente Demande Connection (4) ======
		printf("En attente d'une connexion...\n");
		longueurAdresse = sizeof(pointDeRencontreDistant);
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0)
		{
			perror("Erreur lors de l'acceptation...");
			close(socketEcoute);
			exit(-4);
		}
		printf("Connexion acceptée.\n");

		// ====== Réception Demande (5) ======
		memset(messageRecu, 0x00, LG_MESSAGE);
		nb = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);
		if (nb <= 0)
		{
			perror("Erreur lors de la réception...");
			close(socketDialogue);
			continue;
		}
		printf("Demande reçue : %s\n", messageRecu);

		// ====== Traitement Demande (6) ======
		if (strcmp(messageRecu, "heure") == 0)
		{
			lire_heure(messageEnvoye);
		}
		else if (strcmp(messageRecu, "date") == 0)
		{
			lire_date(messageEnvoye);
		}
		else
		{
			strncpy(messageEnvoye, "Demande invalide.", LG_MESSAGE);
		}

		// ====== Envoi Réponse (7) ======
		send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);
		printf("Réponse envoyée : %s\n", messageEnvoye);

		// ====== Fermeture Socket Dialogue (8) ======
		close(socketDialogue);
		printf("Socket de dialogue fermée.\n");
	}

	// ====== Fermeture Socket Ecoute (9) ======
	close(socketEcoute);
	printf("Socket d'écoute fermée.\n");

	return 0;
}
