static int depth = 13;

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bot.h"
#include <time.h>

long main(int argc, char *argv[])
{

    printf("depth %d", depth);

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

    Board children[42];
    int count = getChildren(b, children);

    time_t t = clock();
    Node *root = createNode(b);

    Board b2 = choose_best_move(b, depth);

    t = clock() - t;
    double timeTaken = ((double)t) / CLOCKS_PER_SEC;

    int a;

    if (b.turn == 0)
        a = b.whites ^ b2.whites;
    else
        a = b.blacks ^ b2.blacks;

    return (b2.turn) | (a << 1);
}
