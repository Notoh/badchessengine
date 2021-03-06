#include "search.h"
#include "tt.h"
#include "uci.h"
#include "io.h"
#include "util.h"
#include "evaluate.h"
#include <stdio.h>
#include "makemove.h"
#include "movegen.h"
#include "attack.h"
#include <stdlib.h>
#include <math.h>

int LMR[64][64];

void initReductions() {
    //terms from ethereal
    for(int depth = 1; depth < 64; depth++) {
        for(int played = 1; played < 64; played++) {
            LMR[depth][played] = 0.75 + log(depth) * log(played) / 2.25;
        }
    }
}

static void checkUp(S_SEARCHINFO *info) {
    if(info->timeset == TRUE && getTimeMs() > info->stop) {
        info->stopped = TRUE;
    }

    ReadInput(info);
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

static bool isRepetition(const S_BOARD *pos) {

    for(int i = pos->histPly - pos->fiftyMove; i<pos->histPly-1; i++) { //ply + 2 because position isn't same if different turn
        if(pos->posKey == pos->history[i].posKey) {
            return true;
        }
    }
    return false;
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

    pos->hashtable->overwrite = 0;
    pos->hashtable->hit = 0;
    pos->hashtable->cut = 0;

    pos->ply = 0;

    info->nullCut = 0;
    info->stopped = 0;
    info->nodes = 0;
}

//based on weiss
static inline bool pawnOn7th(S_BOARD *pos) {
    return pos->pawns[pos->side] & rankBBMask[pos->side == WHITE ? RANK_7 : RANK_1];
}

static int deltaPruningMargin(S_BOARD *pos) {

    const int base = pawnOn7th(pos) ? pieceVal[wQ] : pieceVal[wP];
    int max = pieceVal[wP];

    for(int piece = wP + 6 * pos->side; piece <= wQ + 6 * pos->side; piece++) {
        if(pos->pList[piece][0] != NO_SQ) {
            max = MAX(max, pieceVal[piece]);
        }
    }

    return base + max;

}

static int quiescence(S_BOARD *pos, S_SEARCHINFO *info, int alpha, int beta) {
    ASSERT(checkBoard(pos));
    if((info->nodes & 2047) == 0) {
        checkUp(info);
    }

    info->nodes++;

    if(isRepetition(pos) || pos->fiftyMove >= 100) {
        return 0;
    }

    if(pos->ply > MAXDEPTH - 1) {
        return eval(pos);
    }

    //standing pat
    int score = eval(pos);

    //if score beats beta, assume we have a beta cutoff
    if(score >= beta) {
        return score;
    }
    if(score + deltaPruningMargin(pos) < alpha) {
        return alpha;
    }
    if(score > alpha) {
        alpha = score;
    }


    S_MOVELIST list[1];
    generateAllCaps(pos, list);

    int moveNum = 0;
    int legal = 0;
    score = -INFINITE;

    for(moveNum = 0; moveNum < list->count; moveNum++) {

        pickNextMove(moveNum, list);

        if (!makeMove(pos, list->moves[moveNum].move)) {
            continue;
        }

        legal++;
        score = -quiescence(pos, info, -beta, -alpha);

        takeMove(pos);
        if (score > alpha) {
            if (score >= beta) {
                return beta;
            }
            alpha = score;
        }
    }
    return alpha;
}

static int alphabeta(S_BOARD *pos, S_SEARCHINFO *info, int alpha, int beta, int depth, int doNull) {
    ASSERT(checkBoard(pos));

    int inCheck = sqAttacked(pos->kingSq[pos->side], pos->side^1, pos);
    if(inCheck) {
        depth++;
    }

    const bool pvNode = alpha != beta - 1;
    const bool root = pos->ply == 0;


    if(depth <= 0) {
        return quiescence(pos, info, alpha, beta);
    }

    if((info->nodes & 2047) == 0) {
        checkUp(info);
    }

    info->nodes++;

    //exit early
    if(!root) {
        if((isRepetition(pos) || pos->fiftyMove >= 100)) {
            return 0;
        }

        if(pos->ply >= MAXDEPTH) {
            return eval(pos);
        }

        //mate distance pruning
        alpha = MAX(alpha, -INFINITE+pos->ply);
        beta = MIN(beta, INFINITE - pos->ply-1);
        if(alpha >= beta) {
            return alpha;
        }
    }

    int score = -INFINITE;
    int pvMove = NOMOVE;


    //tt probe
    if(probeHashEntry(pos, &pvMove, &score, alpha, beta, depth) == TRUE) {
        pos->hashtable->cut++;
        return score;
    }

    //logic from weiss
    int stat = pos->history[pos->histPly].score = inCheck ? NOSCORE : !doNull ? -pos->history[pos->histPly-1].score : eval(pos);
    // no 2 null moves in a row, so if we aren't doing it this ply then necessarily we did it last ply

    bool improving = !inCheck && pos->ply >= 2 && stat > pos->history[pos->histPly-2].score;

    //TODO: tune prunings

    //razoring
    if(!root && !inCheck && !pvNode && depth < 2 && stat + 640 < alpha) {
        return quiescence(pos, info, alpha, beta);
    }

    //reverse futility, if eval is well above beta then we assume it will hold above beta
    if(!root && !inCheck && !pvNode && depth <= 8 && stat - 91 * depth > beta) {
        return stat;
    }

    //nullmove
    if(!root && !inCheck && !pvNode && stat >= beta && doNull && (pos->bigPce[pos->side] > 1) && depth >= 3) {
        makeNullMove(pos);
        score = -alphabeta(pos, info, -beta, -beta + 1, depth-4, FALSE);
        takeNullMove(pos);
        if(info->stopped == TRUE) {
            return 0;
        }
        if(score >= beta && abs(score) < ISMATE) {
            info->nullCut++;
            return beta;
        }
    }

    S_MOVELIST list[1];
    generateAllMoves(pos, list);

    int moveNum = 0;
    int quietCount = 0;
    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    score = -INFINITE;

    int quiets[32] = {0};

    if(pvMove != NOMOVE) {
        for(moveNum = 0; moveNum < list->count; moveNum++) {
            if(list->moves[moveNum].move == pvMove) {
                list->moves[moveNum].score = 200000000;
                break;
            }
        }
    }

    for(moveNum = 0; moveNum < list->count; moveNum++) {


        pickNextMove(moveNum, list);

        bool quiet = moveQuiet(list->moves[moveNum].move);

        //late move pruning
        if(!pvNode && !inCheck && quietCount > (3+2*depth*depth) / (2 - improving)) {
            break;
        }

        if(!makeMove(pos, list->moves[moveNum].move)) {
            continue;
        }

        legal++;
        if(quiet && quietCount < 32)
            quiets[quietCount++] = list->moves[moveNum].move;

        //lmr tuning from combination of weiss + ethereal
        bool doLMR = depth > 2 && legal > (2 + pvNode);

        if(doLMR) {
            //base reduction
            int R = LMR[MIN(depth, 63)][MIN(legal, 63)];

            R -= pvNode;

            R -= improving;

            R += quiet;

            int reducedDepth = CLAMP(depth-1-R, 1, depth-1-1);

            score = -alphabeta(pos, info, -alpha - 1, -alpha, reducedDepth, true);
        }

        if(doLMR ? score > alpha : !pvNode || legal > 1)
            score = -alphabeta(pos, info, -alpha - 1, -alpha, depth-1, TRUE);

        if(pvNode && ((score > alpha && score < beta) || legal == 1))
            score = -alphabeta(pos, info, -beta, -alpha, depth-1, TRUE);

        takeMove(pos);

        if(info->stopped == TRUE) {
            return 0;
        }


        //new best move
        if(score > bestScore) {
            bestScore = score;
            bestMove = list->moves[moveNum].move;
            if(score > alpha) {
                //beta cutoff
                if(score >= beta) {

                    //update search killers
                    if(quiet && pos->searchKillers[0][pos->ply] != list->moves[moveNum].move) {
                        pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                        pos->searchKillers[0][pos->ply] = list->moves[moveNum].move;
                    }

                    //update hash table
                    storeHashEntry(pos, bestMove, beta, HFBETA, depth);

                    return beta;
                }
                alpha = score;

                //update history
                if(quiet) {
                    pos->searchHistory[pos->pieces[FROMSQ(bestMove)]][TOSQ(bestMove)] += depth * depth;
                }
            }
        }
    }



    if(legal == 0) {
        if (inCheck) {
            return -INFINITE + pos->ply;
        } else {
            return 0;
        }
    }

    //quiet moves that fail to produce a cutoff get worse history score
    if(bestScore >= beta && moveQuiet(bestMove)) {
        for(int i = 0; i < quietCount; i++) {
            int move = quiets[i];
            if(move == bestMove) continue;
            pos->searchHistory[pos->pieces[FROMSQ(bestMove)]][TOSQ(bestMove)] -= depth * depth;
        }
    }

    if(alpha != oldAlpha) {
        storeHashEntry(pos, bestMove, bestScore, HFEXACT, depth);
    } else {
        storeHashEntry(pos, bestMove, alpha, HFALPHA, depth);
    }

    return alpha;
}

void searchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    int currentDepth = 0;
    int pvMoves = 0;

    clearForSearch(pos, info);
    for(currentDepth = 1; currentDepth <= info->depth; currentDepth++) {
        // alpha	 beta
        bestScore = alphabeta(pos, info, -INFINITE, INFINITE, currentDepth, TRUE);

        if(info->stopped == TRUE) {
            break;
        }

        // out of time?

        pvMoves = getPvLine(pos, currentDepth);
        bestMove = pos->pvarray[0];
#ifndef CONSOLE
        printThinking(pos, currentDepth, bestScore, info->nodes, getTimeMs()-info->start, pvMoves);
#else
        printf("score:%d depth:%d nodes:%ld time:%ld(ms) ", bestScore, currentDepth, info->nodes, getTimeMs()-info->start);
#endif
    }

#ifndef CONSOLE
        printConclusion(pos, bestMove);
#else
        printf("\n\n***!! BCE makes move %s !!***\n\n", prmove(bestMove));
        makeMove(pos, bestMove);
        printBoard(pos);
#endif
}