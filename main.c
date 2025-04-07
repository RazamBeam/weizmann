static int depth = 10;

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bot.h"

long main(int argc, char *argv[])
{

    if (argc != 3)
        return 0;

    Board b = newBoard();

    b.turn = *argv[1] == 'b'; // set turn

    char *temp = argv[2];
    unsigned int p = 1;

    while (*temp)
    {
        switch (*temp)
        {
        case 'w':
            b.whites |= p;
            break;
        case 'W':
            b.whites |= p;
            b.kings |= p;
            break;
        case 'b':
            b.blacks |= p;
            break;
        case 'B':
            b.blacks |= p;
            b.kings |= p;
            break;
        }
        temp++;
        p <<= 1;
    }

    updateValidMoves(&b);

    Board b2 = choose_best_move(b, depth);

    while (b2.turn == b.turn) {
        updateValidMoves(&b2);
        b2 = choose_best_move(b2, depth);
    }
    
    return b.whites ^ b2.whites | b.blacks ^ b2.blacks;
}
