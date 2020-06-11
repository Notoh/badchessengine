#include "movegen.h"
#include "makemove.h"
#include "attack.h"

const int loopSlidePce[8] = {wB, wR, wQ, 0, bB, bR, bQ, 0};

const int loopNonSlidePce[6] = {wN, wK, 0, bN, bK};

const int pceDir[13][8] = {
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0 },
        { -8, -19,	-21, -12, 8, 19, 21, 12 },
        { -9, -11, 11, 9, 0, 0, 0, 0 },
        { -1, -10,	1, 10, 0, 0, 0, 0 },
        { -1, -10,	1, 10, -9, -11, 11, 9 },
        { -1, -10,	1, 10, -9, -11, 11, 9 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { -8, -19,	-21, -12, 8, 19, 21, 12 },
        { -9, -11, 11, 9, 0, 0, 0, 0 },
        { -1, -10,	1, 10, 0, 0, 0, 0 },
        { -1, -10,	1, 10, -9, -11, 11, 9 },
        { -1, -10,	1, 10, -9, -11, 11, 9 }
};


const int numDir[13] = {
        0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

const int victimScore[13] = {0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };

static int mvvLvaScores[13][13];

void initMvvLva() {
    int attacker;
    int victim;
    for(attacker = wP; attacker <= bK; attacker++) {
        for(victim = wP; victim <= bK; victim++) {
            mvvLvaScores[victim][attacker] = victimScore[victim] + 6 - (victimScore[attacker] / 100);
        }
    }
}

int moveExists(S_BOARD *pos, const int move) {

    S_MOVELIST list[1];
    generateAllMoves(pos,list);

    int MoveNum = 0;
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        if (!makeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        takeMove(pos);
        if(list->moves[MoveNum].move == move) {
            return TRUE;
        }
    }
    return FALSE;
}

static void addQuietMove(const S_BOARD *pos, int move, S_MOVELIST *movelist) {

    ASSERT(sqOnBoard(FROMSQ(move)));
    ASSERT(sqOnBoard(FROMSQ(move)));

    movelist->moves[movelist->count].move = move;

    if(pos->searchKillers[0][pos->ply] == move) {
        movelist->moves[movelist->count].score = 900000;
    } else if(pos->searchKillers[1][pos->ply] == move) {
        movelist->moves[movelist->count].score = 800000;
    } else {
        movelist->moves[movelist->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
    }

    movelist->count++;
}

static void addCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *movelist) {
    ASSERT(sqOnBoard(FROMSQ(move)));
    ASSERT(sqOnBoard(TOSQ(move)));
    ASSERT(pieceValid(CAPTURED(move)));

    movelist->moves[movelist->count].move = move;
    movelist->moves[movelist->count].score = mvvLvaScores[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1000000;
    movelist->count++;
}

static void addEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *movelist) {
    ASSERT(sqOnBoard(FROMSQ(move)));
    ASSERT(sqOnBoard(TOSQ(move)));

    movelist->moves[movelist->count].move = move;
    movelist->moves[movelist->count].score = 1000105;
    movelist->count++;
}

static void addWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {

    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));
    ASSERT(pieceValidEmpty(cap));

    if(ranksBrd[from] == RANK_7) {
        addCaptureMove(pos, MOVE(from, to, cap, wQ, 0), list);
        addCaptureMove(pos, MOVE(from, to, cap, wR, 0), list);
        addCaptureMove(pos, MOVE(from, to, cap, wB, 0), list);
        addCaptureMove(pos, MOVE(from, to, cap, wN, 0), list);
    } else {
        addCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
    }
}

static void addWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {

    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));

    if(ranksBrd[from] == RANK_7) {
        addQuietMove(pos, MOVE(from, to, EMPTY, wQ, 0), list);
        addQuietMove(pos, MOVE(from, to, EMPTY, wR, 0), list);
        addQuietMove(pos, MOVE(from, to, EMPTY, wB, 0), list);
        addQuietMove(pos, MOVE(from, to, EMPTY, wN, 0), list);
    } else {
        addQuietMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);
    }
}

static void addBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {

    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));
    ASSERT(pieceValidEmpty(cap));

    if(ranksBrd[from] == RANK_2) {
        addCaptureMove(pos, MOVE(from, to, cap, bQ, 0), list);
        addCaptureMove(pos, MOVE(from, to, cap, bR, 0), list);
        addCaptureMove(pos, MOVE(from, to, cap, bB, 0), list);
        addCaptureMove(pos, MOVE(from, to, cap, bN, 0), list);
    } else {
        addCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
    }
}

static void addBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {

    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));

    if(ranksBrd[from] == RANK_2) {
        addQuietMove(pos, MOVE(from, to, EMPTY, bQ, 0), list);
        addQuietMove(pos, MOVE(from, to, EMPTY, bR, 0), list);
        addQuietMove(pos, MOVE(from, to, EMPTY, bB, 0), list);
        addQuietMove(pos, MOVE(from, to, EMPTY, bN, 0), list);
    } else {
        addQuietMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);
    }
}

void generateAllMoves(const S_BOARD *pos, S_MOVELIST *movelist) {
    ASSERT(checkBoard(pos));

    movelist->count=0;

    int pce = EMPTY;
    int side = pos->side;
    int sq = 0;
    int t_sq = 0;
    int pceNum = 0;
    int dir = 0;
    int index = 0;
    int pceIndex = 0;

    //pawns + castling
    if(side == WHITE) {

        for(pceNum = 0; pceNum < pos->pceNum[wP]; pceNum++) {
            sq = pos->pList[wP][pceNum];
            ASSERT(sqOnBoard(sq));

            if(pos->pieces[sq + 10] == EMPTY) {
                addWhitePawnMove(pos, sq, sq+10, movelist);
                if(ranksBrd[sq] == RANK_2 && pos->pieces[sq+20] == EMPTY) {
                    addQuietMove(pos, MOVE(sq, (sq+20), EMPTY, EMPTY, MFLAGPS), movelist);
                }
            }

            if(!SQOFFBOARD(sq + 9) && pieceCol[pos->pieces[sq + 9]] == BLACK) {
                addWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], movelist);
            }
            if(!SQOFFBOARD(sq + 11) && pieceCol[pos->pieces[sq+11]] == BLACK) {
                addWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], movelist);
            }
            if(pos->enPas != NO_SQ) {
                if(sq + 9 == pos->enPas) {
                    addEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEP), movelist);
                } else if(sq + 11 == pos->enPas) { //if enPas exists it cant exist on the other, so if else saves a few cycles sometimes
                    addEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEP), movelist);
                }
            }
        }
        //castling
        if(pos->castlePerm & WSCA) {
            if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY && !sqAttacked(E1, BLACK, pos) &&
               !sqAttacked(F1, BLACK, pos)) {
                addQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), movelist);
            }
        }

        if(pos->castlePerm & WLCA) {
            if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY && !sqAttacked(E1, BLACK, pos) &&
               !sqAttacked(D1, BLACK, pos)) {
                addQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), movelist);
            }
        }


    } else {
        for(pceNum = 0; pceNum < pos->pceNum[bP]; pceNum++) {
            sq = pos->pList[bP][pceNum];
            ASSERT(sqOnBoard(sq));

            if(pos->pieces[sq - 10] == EMPTY) {
                addBlackPawnMove(pos, sq, sq-10, movelist);
                if(ranksBrd[sq] == RANK_7 && pos->pieces[sq-20] == EMPTY) {
                    addQuietMove(pos, MOVE(sq, (sq-20), EMPTY, EMPTY, MFLAGPS), movelist);
                }
            }

            if(!SQOFFBOARD(sq - 9) && pieceCol[pos->pieces[sq - 9]] == WHITE) {
                addBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq-9], movelist);
            }
            if(!SQOFFBOARD(sq - 11) && pieceCol[pos->pieces[sq-11]] == WHITE) {
                addBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq-11], movelist);
            }
            if(pos->enPas != NO_SQ) {
                if(sq - 9 == pos->enPas) {
                    addEnPassantMove(pos, MOVE(sq, sq-9, EMPTY, EMPTY, MFLAGEP), movelist);
                } else if(sq - 11 == pos->enPas) { //if enPas exists it cant exist on the other, so else saves a few cycles sometimes
                    addEnPassantMove(pos, MOVE(sq, sq-11, EMPTY, EMPTY, MFLAGEP), movelist);
                }
            }
        }
        //castling
        if(pos->castlePerm & BSCA) {
            if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY && !sqAttacked(E8, WHITE, pos) &&
               !sqAttacked(F8, WHITE, pos)) {
                addQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), movelist);
            }
        }

        if(pos->castlePerm & BLCA) {
            if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY && !sqAttacked(E8, WHITE, pos) &&
               !sqAttacked(D8, WHITE, pos)) {
                addQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), movelist);
            }
        }
    }


    //slide

    pceIndex = side*4;
    pce = loopSlidePce[pceIndex++];
    while(pce != 0) {
        ASSERT(pieceValid(pce));

        for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++) {
            sq = pos->pList[pce][pceNum];
            ASSERT(sqOnBoard(sq));

            for(index = 0; index < numDir[pce]; index++) {
                dir = pceDir[pce][index];
                t_sq = sq + dir;

                while(!SQOFFBOARD(t_sq)) {
                    if(pos->pieces[t_sq] != EMPTY) {
                        if(pieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                            addCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), movelist);
                        }
                        break;
                    }
                    addQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), movelist);
                    t_sq += dir;
                }
            }
        }
        pce = loopSlidePce[pceIndex++];
    }
    //nonslide
    pceIndex = side*3;
    pce = loopNonSlidePce[pceIndex++];
    while(pce != 0) {
        ASSERT(pieceValid(pce));
        for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++) {
            sq = pos->pList[pce][pceNum];
            ASSERT(sqOnBoard(sq));

            for(index = 0; index < numDir[pce]; index++) {
                dir = pceDir[pce][index];
                t_sq = sq + dir;

                if (SQOFFBOARD(t_sq)) {
                    continue;
                }

                if (pos->pieces[t_sq] != EMPTY) {
                    if (pieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                        addCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), movelist);
                    }
                    continue;
                }
                addQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), movelist);
            }
        }
        pce = loopNonSlidePce[pceIndex++];
    }
}

void generateAllCaps(const S_BOARD *pos, S_MOVELIST *movelist) {
    ASSERT(checkBoard(pos));

    movelist->count=0;

    int pce = EMPTY;
    int side = pos->side;
    int sq = 0;
    int t_sq = 0;
    int pceNum = 0;
    int dir = 0;
    int index = 0;
    int pceIndex = 0;

    //pawns + castling
    if(side == WHITE) {

        for(pceNum = 0; pceNum < pos->pceNum[wP]; pceNum++) {
            sq = pos->pList[wP][pceNum];
            ASSERT(sqOnBoard(sq));


            if(!SQOFFBOARD(sq + 9) && pieceCol[pos->pieces[sq + 9]] == BLACK) {
                addWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], movelist);
            }
            if(!SQOFFBOARD(sq + 11) && pieceCol[pos->pieces[sq+11]] == BLACK) {
                addWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], movelist);
            }
            if(pos->enPas != NO_SQ) {
                if(sq + 9 == pos->enPas) {
                    addEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEP), movelist);
                } else if(sq + 11 == pos->enPas) { //if enPas exists it cant exist on the other, so if else saves a few cycles sometimes
                    addEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEP), movelist);
                }
            }
        }
    } else {
        for(pceNum = 0; pceNum < pos->pceNum[bP]; pceNum++) {
            sq = pos->pList[bP][pceNum];
            ASSERT(sqOnBoard(sq));

            if(!SQOFFBOARD(sq - 9) && pieceCol[pos->pieces[sq - 9]] == WHITE) {
                addBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq-9], movelist);
            }
            if(!SQOFFBOARD(sq - 11) && pieceCol[pos->pieces[sq-11]] == WHITE) {
                addBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq-11], movelist);
            }
            if(pos->enPas != NO_SQ) {
                if(sq - 9 == pos->enPas) {
                    addEnPassantMove(pos, MOVE(sq, sq-9, EMPTY, EMPTY, MFLAGEP), movelist);
                } else if(sq - 11 == pos->enPas) { //if enPas exists it cant exist on the other, so else saves a few cycles sometimes
                    addEnPassantMove(pos, MOVE(sq, sq-11, EMPTY, EMPTY, MFLAGEP), movelist);
                }
            }
        }
    }


    //slide

    pceIndex = side*4;
    pce = loopSlidePce[pceIndex++];
    while(pce != 0) {
        ASSERT(pieceValid(pce));

        for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++) {
            sq = pos->pList[pce][pceNum];
            ASSERT(sqOnBoard(sq));

            for(index = 0; index < numDir[pce]; index++) {
                dir = pceDir[pce][index];
                t_sq = sq + dir;

                while(!SQOFFBOARD(t_sq)) {
                    if(pos->pieces[t_sq] != EMPTY) {
                        if(pieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                            addCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), movelist);
                        }
                        break;
                    }
                    t_sq += dir;
                }
            }
        }
        pce = loopSlidePce[pceIndex++];
    }
    //nonslide
    pceIndex = side*3;
    pce = loopNonSlidePce[pceIndex++];
    while(pce != 0) {
        ASSERT(pieceValid(pce));
        for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++) {
            sq = pos->pList[pce][pceNum];
            ASSERT(sqOnBoard(sq));

            for(index = 0; index < numDir[pce]; index++) {
                dir = pceDir[pce][index];
                t_sq = sq + dir;

                if (SQOFFBOARD(t_sq)) {
                    continue;
                }

                if (pos->pieces[t_sq] != EMPTY) {
                    if (pieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                        addCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), movelist);
                    }
                    continue;
                }
            }
        }
        pce = loopNonSlidePce[pceIndex++];
    }
}
