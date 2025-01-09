#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void start_game_in_terminal()
{
    char command_server[256];
    char command_client[256];

    snprintf(command_server, sizeof(command_server), "xterm -hold -e ./serveur ");
    snprintf(command_client, sizeof(command_client), "xterm -hold -e ./client 127.0.0.1 5000 ");
    int fils = fork();
    if (fils == 0)
    {
        system(command_server);
        exit(0);
    }
    // Lancer le premier client dans un autre terminal
    int fils2 = fork();
    if (fils2 == 0)
    {
        sleep(1);
        system(command_client);
        exit(0);
    }
    // Lancer le second client dans un autre terminal
    int fils3 = fork();
    if (fils3 == 0)
    {
        sleep(2);
        system(command_client);
        exit(0);
    }
}

int main()
{
    char command_server[265];
    int nbPartie;

    printf("Vous voulez combien de partie ?\n");
    scanf(" %d", &nbPartie);

    for (int i = 1; i <= nbPartie; i++)
    {
        int fils = fork();
        if (fils == 0)
        {
            snprintf(command_server, sizeof(command_server), "xterm -hold -e ./serveur");
            system(command_server);
        }
    }
    return 0;
}
