#ifndef MORPION_H
#define MORPION_H

#include <stdbool.h>

typedef struct
{
    char grille[3][3];
} Morpion;

bool isFull(const Morpion *m);
void initialise(Morpion *m);
void show(const Morpion *m);
bool isValid(Morpion *m, int cell);
void place(Morpion *m, int cell, char form);

char* whoWin(Morpion *m, int x, int y);
char* checkWin(Morpion *m);

void jeuClient(int descripteurSocket);
void jeuServeur(int socketClient1, int socketClient2,int socketSpectateur);
void jeuSpectateur(int descripteurSocket);
void verifEnvoye(ssize_t bytesSent, const char *messageEnvoye);
bool verifRecu(ssize_t bytesReceived, char *messageRecu);
char *getCaseVide(Morpion *m);

#endif