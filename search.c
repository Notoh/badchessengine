#include "defs.h"

#define INFINITE 30000
#define MATE 29000

static void checkUp() {
    //check interrupt
}

static void pickNextMove(int moveNum, S_MOVELIST *list) {
    S_MOVE temp;
    int index = 0;
    int bestScore = 0;
    int bestNum = moveNum;
    for(index = moveNum; index < list->count; index++) {
        if(list->moves[index].score > bestScore) {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }
    temp = list->moves[moveNum];
    list->moves[moveNum] = list->moves[bestNum];
    list->moves[bestNum]= temp;
}

static int isRepetition(const S_BOARD *pos) {
    int index = 0;

    for(index = pos->histPly - pos->fiftyMove; index<pos->histPly-1; index++) {

        ASSERT(index >= 0 && index < MAXGAMEMOVES);

        if(pos->posKey == pos->history[index].posKey) {
            return TRUE;
        }
    }
    return FALSE;
}

static void clearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
    int i = 0;
    int j = 0;

    for(i = 0; i < 13; i++) {
        for(j = 0; j < BRD_SQ_NUM; j++) {
            pos->searchHistory[i][j] = 0;
        }
    }

    for(i = 0; i < 2; i++) {
        for(j = 0; j < MAXDEPTH; j++) {
            pos->searchKillers[i][j] = 0;
        }
    }

    clearPvTable(pos->pvtable);
    pos->ply = 0;

    info->start = getTimeMs();
    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}



static int quiescence(S_BOARD *pos, S_SEARCHINFO *info, int alpha, int beta) {
    return 0;
}

static int alphabeta(S_BOARD *pos, S_SEARCHINFO *info, int alpha, int beta, int depth, int doNull) {
    ASSERT(checkBoard(pos));

    if(depth == 0) {
        info->nodes++;
        return eval(pos);
    }

    info->nodes++;

    if(isRepetition(pos) || pos->fiftyMove >= 100) {
        return 0;
    }

    if(pos->ply > MAXDEPTH - 1) {
        return eval(pos);
    }

    S_MOVELIST list[1];
    generateAllMoves(pos, list);

    int moveNum = 0;
    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = NOMOVE;
    int score = -INFINITE;

    for(moveNum = 0; moveNum < list->count; moveNum++) {

        pickNextMove(moveNum, list);

        if(!makeMove(pos, list->moves[moveNum].move)) {
            continue;
        }

        legal++;
        score = -alphabeta(pos, info, -beta, -alpha, depth-1, TRUE);

        takeMove(pos);
        if(score > alpha) {
            if(score >= beta) {
                if(legal == 1) {
                    info->fhf++;
                }
                info->fh++;
                return beta;
            }
            alpha = score;
            bestMove = list->moves[moveNum].move;
        }
    }

    if(legal == 0) {
        if (sqAttacked(pos->kingSq[pos->side], pos->side ^ 1, pos)) {
            return -MATE + pos->ply;
        } else {
            return 0;
        }
    }

    if(alpha != oldAlpha) {
        storePvMove(pos, bestMove);
    }

    return alpha;
}

void searchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    int currentDepth = 0;
    int pvMoves = 0;
    int pvNum = 0;
    clearForSearch(pos, info);
    for(currentDepth = 1; currentDepth <= info->depth; currentDepth++) {
        // alpha	 beta
        bestScore = alphabeta(pos, info, -INFINITE, INFINITE, currentDepth, TRUE);

        // out of time?

        pvMoves = getPvLine(pos, currentDepth);
        bestMove = pos->pvarray[0];

        printf("Depth:%d score:%d move:%s nodes:%ld ",
               currentDepth,bestScore,prmove(bestMove),info->nodes);

        pvMoves = getPvLine(pos, currentDepth);
        printf("pv");
        for(pvNum = 0; pvNum < pvMoves; pvNum++) {
            printf(" %s",prmove(pos->pvarray[pvNum]));
        }
        printf("\n");
        printf("Ordering:%.2f\n",(info->fhf/info->fh));
    }
}