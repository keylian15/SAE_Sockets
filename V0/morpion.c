#include <stdbool.h>
#include <stdio.h>
#include "morpion.h"



bool isFull(Morpion *m)
{
    for (int i = 0; i < 3; i++)
    {
        /* code */
    }
    
}


void initialise(){

}
void show(){

}
bool isValid(cell){
    if (cell>3 || cell<3)
    {
       return false;
    }
    else{
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
void place(Morpion *m,int cellx,int celly,char form){
    return m->grille[cellx][celly]=form;
}
