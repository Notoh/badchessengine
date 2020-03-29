#include "defs.h"



int main() {
    init();

    S_BOARD board[1];
    S_MOVELIST list[1];

    parseFEN(START_FEN,board);

    char input[6];

    int move = NOMOVE;
    while(TRUE) {
        printBoard(board);
        printf("Please enter a move > ");
        fgets(input, 6, stdin);

        if(input[0]=='q') {
            break;
        } else if(input[0] == 't') {
            takeMove(board);
        } else {
            move = parsemove(input, board);
            if(move != NOMOVE) {
                makeMove(board, move);
            }
        }

        fflush(stdin);
    }

    return 0;
}
