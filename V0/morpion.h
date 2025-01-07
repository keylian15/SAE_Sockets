#ifndef MORPION_H
#define MORPION_H

#include <stdbool.h>

typedef struct
{
    char grille[3][3];
} Morpion;

bool isFull(const Morpion *m);

#endif