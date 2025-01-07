#include <stdio.h>
#include "morpion.c"

int main()
{
    Morpion jeu = {{{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}}};
    show(&jeu);
}