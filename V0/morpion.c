#include <stdbool.h>
#include <stdio.h>
#include "morpion.h"

/**
 * Vérifie si la grille est entièrement remplie.
 * isFull(&m);
 * @param m Pointeur vers la structure Morpion.
 * @return true si la grille est pleine, false sinon.
 */
bool isFull(Morpion *m)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (m->grille[i][j] == ' ') 
            {
                return false;
            }
        }
    }
    return true;
}

