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
void show(const Morpion *m)
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

/**
 * verifie si la cellule existe
 * isValid(2);
 * @param cell numero de la cellule
 * @return bool
 */
bool isValid(int cell)
{
    if (cell > 9 || cell < 1)
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
 * place(&m,3,x);
 * @param cell numero de la cellule
 * @param form la forme qui est entrée
 */
void place(Morpion *m, int cell, char form)
{
    int count = 1;
    for (int cellx = 0; cellx < 3; cellx++)
    {
        for (int celly = 0; celly < 3; celly++)
        {
            if (count == cell)
            {
                m->grille[cellx][celly] = form;
                return;
            }
            count++;
        }
    }
}

/**
 * verifie qui a gagner a l'aide d'une combinaison de coordonée
 * whoWin(&m,1,1); *
 * @return char
 */
char whoWin(Morpion *m, int x, int y)
{

    if (m->grille[x][y] == "x")
    {
        return "le gagnant est le client";
    }
    else
    {
        return "le gagnant est le serveur";
    }
}
/**
 * verifie si il y a un gagnant dans la grille et renvoie le gagnant si oui
 * checkwin(&m);
 * @return char
 */
char checkWin(Morpion *m)
{
    for (int i = 0; i < 3; i++)
    {
        if (m->grille[i][1] == m->grille[i][0] && m->grille[i][2] == m->grille[i][0])
        {
            return whoWin(&m, i, 1);
        }
    }
    for (int j = 0; j < 3; j++)
    {
        if (m->grille[0][j] == m->grille[1][j] && m->grille[2][j] == m->grille[0][j])
        {
            return whoWin(&m, 1, j);
        }
    }
    if (m->grille[0][0] == m->grille[1][1] && m->grille[2][2] == m->grille[1][1])
    {
        return whoWin(&m, 1, 1);
    }
    else if (m->grille[0][3] == m->grille[1][1] && m->grille[2][0] == m->grille[1][1])
    {
        return whoWin(&m, 1, 1);
    }
    else
    {
        return "pas de gagnant on continue";
    }
}
