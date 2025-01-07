#include <stdbool.h>
#include <stdio.h>
#include "morpion.h"

/**
 * Vérifie si la grille est entièrement remplie.
 * isFull(&m);
 * @param m Pointeur vers la structure Morpion.
 * @return true si la grille est pleine, false sinon.
 */

bool isFull(const Morpion *m)
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

/**
 * Initialise la grille en remplissant toutes les cases avec des espaces.
 * @param m Pointeur vers la structure Morpion.
 */
void initialise(Morpion *m)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            m->grille[i][j] = ' ';
        }
    }
}

/**
 * Afficher la grille
 * show(&m);
 * @param m Pointeur vers la structure Morpion.
 */
void show(Morpion *m)
{
    int count = 1;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (m->grille[i][j] == ' ')
            {
                printf("%d", count);
            }
            else
            {
                printf("%c", m->grille[i][j]);
            }
            if (j < 2)
                printf("|");

            count++;
        }
        printf("\n");
        if (i < 2)
            printf("-----\n");
    }
}

bool isValid(int cell)
{
    if (cell > 3 || cell < 3)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * place dans la grille la forme demannder
 * place(&m,0,2,x)
 * @param cellx coordone x de la cellule
 * @param celly coordone y de la cellule
 * @param form la forme qui est entrée
 */
void place(Morpion *m, int cellx, int celly, char form)
{
    m->grille[cellx][celly] = form;
}
