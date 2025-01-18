static int depth = 7;

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bot.h"
#include <time.h>


int main(int argc, char *argv[])
{

    if (argc != 3)
        return 0;

    Board b = newBoard();

    b.turn = *argv[1] == 'b'; // set turn

    printf("%d\n", b.turn);

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

    // setup(&b);
    printBinary(b.whites);
    printf("\n");
    printBinary(b.blacks);
    printf("\n");
    printBinary(b.kings);
    printf("\n");

    time_t t = clock();
    Node *root = createNode(b);
    printf("%d\n", buildTree(root, depth));
    Board b2 = choose_best_move(root);
    freeTree(root);
    t = clock() - t;
    double timeTaken = ((double)t) / CLOCKS_PER_SEC;
    printf("time taken - %lf\n", timeTaken);
    printf("depth %d\n", depth);
    int a;

    if (b.turn == 0)
        a = b.whites ^ b2.whites;
    else
        a = b.blacks ^ b2.blacks;

    printBinary(a);

    return a;
}
