#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char command_server[265];
    int nbPartie;
    snprintf(command_server, sizeof(command_server), "xterm -hold -e ./T3N_serveur_V2");
    system(command_server);
    return 1;
}
