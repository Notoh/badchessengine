// perft.c

#include "perft.h"
#include "util.h"
#include "io.h"
#include "makemove.h"
#include "movegen.h"
#include "board.h"

#define PERFTFEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"


long leafNodes;

void perft(S_BOARD *pos, int depth) {

    ASSERT(checkBoard(pos));

    if(depth == 0) {
        leafNodes++;
        return;
    }

    S_MOVELIST list[1];
    generateAllMoves(pos,list);

    int moveNum = 0;
    for(moveNum = 0; moveNum < list->count; moveNum++) {

        if (!makeMove(pos,list->moves[moveNum].move))  {
            continue;
        }
        perft(pos, depth-1);
        takeMove(pos);
    }

    return;
}


void perftTest(S_BOARD *pos, int depth) {

    ASSERT(checkBoard(pos));

    printBoard(pos);
    printf("\nStarting Test To Depth:%d\n",depth);
    leafNodes = 0;
    long start = getTimeMs();
    S_MOVELIST list[1];
    generateAllMoves(pos,list);

    int move;
    int moveNum = 0;
    for(moveNum = 0; moveNum < list->count; moveNum++) {
        move = list->moves[moveNum].move;
        if (!makeMove(pos,move))  {
            continue;
        }

        long cumnodes = leafNodes;
        perft(pos, depth-1);
        takeMove(pos);
        long oldnodes = leafNodes - cumnodes;
        printf("move %d : %s : %ld\n", moveNum + 1, prmove(move), oldnodes);
    }

    printf("\nTest Complete : %ld nodes visited in %ldms\n",leafNodes, getTimeMs() - start);

    return;
}












