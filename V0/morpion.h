#ifndef MORPION_H
#define MORPION_H

typedef struct
{
    char grille[3][3];
} Morpion;

bool isFull(const Morpion *m);

#endif