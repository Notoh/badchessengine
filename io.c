#include "io.h"
#include "movegen.h"
#include <stdio.h>

char *prsq(const int sq) {

    static char sqstr[3];

    int file = filesBrd[sq];
    int rank = ranksBrd[sq];

    sprintf(sqstr, "%c%c", ('a' + file), ('1' + rank));

    return sqstr;
}

char *prmove(const int move) {

    static char mvstr[6];

    int ff = filesBrd[FROMSQ(move)];
    int rf = ranksBrd[FROMSQ(move)];
    int ft = filesBrd[TOSQ(move)];
    int rt = ranksBrd[TOSQ(move)];

    int p = PROMOTED(move);

    if(p) {
        char pchar = 'q';
        if(IsKn(p)) {
           pchar = 'n';
        } else if(IsRQ(p) && !IsBQ(p)) {
            pchar = 'r';
        } else if(!IsRQ(p) && IsBQ(p)) {
            pchar = 'b';
        }
        sprintf(mvstr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
    } else {
        sprintf(mvstr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
    }

    return mvstr;
}

int parsemove(const char *ptrchar, S_BOARD *pos) {
    ASSERT(checkBoard(pos));
    if(ptrchar[1] > '8' || ptrchar[1] < '1') return NOMOVE;
    if(ptrchar[3] > '8' || ptrchar[3] < '1') return NOMOVE;
    if(ptrchar[0] > 'h' || ptrchar[0] < 'a') return NOMOVE;
    if(ptrchar[2] > 'h' || ptrchar[2] < 'a') return NOMOVE;

    int from = FR2SQ(ptrchar[0] - 'a', ptrchar[1] - '1');
    int to = FR2SQ(ptrchar[2] - 'a', ptrchar[3] - '1');

    ASSERT(sqOnBoard(from) && sqOnBoard(to));

    S_MOVELIST list[1];
    generateAllMoves(pos, list);
    int moveNum = 0;
    int move = 0;
    int promPce = 0;
    for(moveNum = 0; moveNum < list->count; ++moveNum) {
        move = list->moves[moveNum].move;
        if(FROMSQ(move) == from && TOSQ(move) == to) {
            promPce = PROMOTED(move);
            if(promPce != EMPTY) {
                if(IsRQ(promPce) && IsBQ(promPce) && ptrchar[4] == 'q') {
                    return move;
                } else if(IsKn(promPce) && ptrchar[4] == 'n') {
                    return move;
                } else if(IsRQ(promPce) && !IsBQ(promPce) && ptrchar[4] == 'r') {
                    return move;
                } else if(!IsRQ(promPce) && IsBQ(promPce) && ptrchar[4] == 'b') {
                    return move;
                }
                continue;
            }
            return move;
        }
    }

    return NOMOVE;

}

void printmvlist(const S_MOVELIST *list) {
    int index = 0;
    int score = 0;
    int move = 0;
    printf("Movelist: %d\n",list->count);

    for(index = 0; index < list->count; ++index) {

        move = list->moves[index].move;
        score = list->moves[index].score;

        printf("Move:%d > %s (score:%d)\n",index+1,prmove(move),score);
    }
    printf("MoveList Total %d Moves:\n\n",list->count);
}