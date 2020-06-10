#include "validate.h"
#include "board.h"
#include "evaluate.h"
#include <string.h>
#include <stdio.h>


int sqIs120(const int sq) {
    return (sq>=0 && sq<120);
}

int pieceValidEmpty(const int pce) {
    return (pce >= EMPTY && pce <= bK) ? 1 : 0;
}

int pceValidEmptyOffbrd(const int pce) {
    return (pieceValidEmpty(pce) || pce == OFFBOARD);
}
int sqOnBoard(const int sq) {
    return filesBrd[sq]==OFFBOARD ? 0 : 1;
}

int sideValid(const int side) {
    return (side==WHITE || side == BLACK) ? 1 : 0;
}

int fileRankValid(const int fr) {
    return (fr >= 0 && fr <= 7) ? 1 : 0;
}


int pieceValid(const int pce) {
    return (pce >= wP && pce <= bK) ? 1 : 0;
}

void mirrorEvalTest(S_BOARD *pos) {
    FILE *file = fopen("mirror.epd","r");
    char lineIn [1024];
    int ev1 = 0; int ev2 = 0;
    int positions = 0;
    if(file == NULL) {
        printf("File Not Found\n");
        return;
    }  else {
        while(fgets (lineIn , 1024 , file) != NULL) {
            parseFEN(lineIn, pos);
            positions++;
            ev1 = eval(pos);
            mirrorBoard(pos);
            ev2 = eval(pos);

            if(ev1 != ev2) {
                printf("\n\n\n");
                parseFEN(lineIn, pos);
                printBoard(pos);
                mirrorBoard(pos);
                printBoard(pos);
                printf("\n\nMirror Fail:\n%s\n",lineIn);
                getchar();
                return;
            }

            if( (positions % 1000) == 0)   {
                printf("position %d\n",positions);
            }

            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}