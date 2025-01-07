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
bool isValid(int cell);
void place(Morpion *m, int cell, char form);

void jeuClient(int descripteurSocket);
void jeuServeur(int socketDialogue);
void verifEnvoye(ssize_t bytesSent, const char *messageEnvoye);
void verifRecu(ssize_t bytesReceived, char *messageRecu);

#endif