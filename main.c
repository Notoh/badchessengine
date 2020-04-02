#include "defs.h"

#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

static S_BOARD *genBoard() {
    S_BOARD *board = (S_BOARD *) malloc(sizeof(S_BOARD));
    board->pvtable->pTable = NULL;
    return board;
}

static void cleanup(S_BOARD *board) {
    free(board->pvtable->pTable);
    free(board);
}

int main(void) {
    init();

    S_BOARD *board = genBoard();
    initPvTable(board->pvtable);

    S_MOVELIST list[1];
    S_SEARCHINFO info[1];
    parseFEN(WAC1,board);


    char input[6];
    int move = NOMOVE;
    int pvNum = 0;
    int max = 0;
    while(TRUE) {
        printBoard(board);
        printf("Please enter a move > ");
        fgets(input, 6, stdin);

        if(input[0]=='q') {
            break;
        } else if(input[0]=='t') {
            takeMove(board);
        } else if(input[0]=='s') {
            info->depth = 5;
            searchPosition(board, info);
        } else {
            move = parsemove(input, board);
            if(move != NOMOVE) {
                storePvMove(board, move);
                makeMove(board, move);
            } else {
                printf("Move Not Parsed:%s\n",input);
            }
        }
        fflush(stdin);
    }

    cleanup(board);

    return 0;

}
