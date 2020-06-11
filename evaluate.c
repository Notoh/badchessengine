#include "evaluate.h"
#include <stdlib.h>

//TODO taper all of these

const int PawnIsolated = -10;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 80, 200, 700 };
const int RookOpenFile = 12;
const int RookSemiOpenFile = 6;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;

const int pieceVal[13] = { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000 };

const int PawnTable[64] = {
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
        10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
        5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
        0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
        5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
        10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
        20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int KnightTable[64] = {
        0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
        0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
        0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
        0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
        5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
        5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int BishopTable[64] = {
        0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
        0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
        0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
        0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
        0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
        0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
        0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
        0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

const int RookTable[64] = {
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
        25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
        0	,	0	,	5	,	10	,	10	,	5	,	0	,	0
};

const int KingE[64] = {
        -50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
        -10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
        0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
        0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
        0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
        0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
        -10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
        -50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50
};

const int KingO[64] = {
        0	,	7	,	5	,	-10	,	-10	,	2	,	10	,	9	,
        -30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-5	,	-9	,
        -50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70
};


//based on logic from sjeng
static int materialDraw(const S_BOARD *pos) {

    ASSERT(checkBoard(pos));

    //not a draw with pawns or queens
    if (pos->pceNum[wP] || pos->pceNum[bP] || pos->pceNum[wQ] || pos->pceNum[bQ]) {
        return FALSE;
    }

    //no rooks
    if (!pos->pceNum[wR] && !pos->pceNum[bR]) {

        //no bishops
        if (!pos->pceNum[wB] && !pos->pceNum[bB]) {
            //0-2 knights v king is draw
            return pos->pceNum[wN] <= 2 && pos->pceNum[bN] <= 2;
        } else if (!pos->pceNum[wN] && !pos->pceNum[bN]) { //no knights
            //draw unless one side has 2 extra bishops
            return abs(pos->pceNum[wB] - pos->pceNum[bB]) < 2;
        } else if ((pos->pceNum[wN] <= 2 && pos->pceNum[bB] == 1) || (pos->pceNum[bN] <= 2 && pos->pceNum[wB] == 1)) {
            return TRUE;
        }
    } else if (pos->pceNum[wR] == 1 && pos->pceNum[bR] == 1) { //1 rook vs 1 rook with 0-1 minors each is draw
        return pos->pceNum[wN] + pos->pceNum[wB] <= 1 && pos->pceNum[bN] + pos->pceNum[bB] <= 1;
    } else if (pos->pceNum[wR] + pos->pceNum[bR] == 1) { //1 rook draws vs 1-2 minors
        if (pos->pceNum[wR]) {
            return pos->pceNum[wR] + pos->pceNum[wN] + pos->pceNum[wB] == 1 && pos->pceNum[bB] + pos->pceNum[bN] >= 1 &&
                   pos->pceNum[bB] + pos->pceNum[bN] <= 2;
        } else {
            return pos->pceNum[bR] + pos->pceNum[bN] + pos->pceNum[bB] == 1 && pos->pceNum[wB] + pos->pceNum[wN] >= 1 &&
                   pos->pceNum[wB] + pos->pceNum[wN] <= 2;
        }
    }
    return FALSE;

}

#define ENDGAME_MAT (1 * pieceVal[wR] + 2 * pieceVal[wN] + 2 * pieceVal[wP] + pieceVal[wK])

int eval(const S_BOARD *pos) {
    ASSERT(checkBoard(pos));

    int pce;
    int pceNum;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];

    if(!pos->pceNum[wP] && !pos->pceNum[bP] && materialDraw(pos) == TRUE) {
        return 0;
    }
    pce = wP;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += PawnTable[SQ64(sq)];

        if((isolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            score += PawnIsolated;
        }

        if((whitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            score += PawnPassed[ranksBrd[sq]];
        }
    }

    pce = bP;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq)) >= 0 && MIRROR64(SQ64(sq)) <= 63);
        score -= PawnTable[MIRROR64(SQ64(sq))];

        if((isolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            score -= PawnIsolated;
        }

        if((blackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            score -= PawnPassed[7-ranksBrd[sq]];
        }
    }

    pce = wN;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        score += KnightTable[SQ64(sq)];
    }

    pce = bN;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        score -= KnightTable[MIRROR64(SQ64(sq))];
    }

    pce = wB;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);
        score += BishopTable[SQ64(sq)];
    }

    pce = bB;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);
        score -= BishopTable[MIRROR64(SQ64(sq))];
    }

    pce = wR;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += RookTable[SQ64(sq)];

        ASSERT(fileRankValid(filesBrd[sq]));
        if(!(pos->pawns[BOTH] & fileBBMask[filesBrd[sq]])) {
            score += RookOpenFile;
        } else if(!(pos->pawns[WHITE] & fileBBMask[filesBrd[sq]])) {
            score += RookSemiOpenFile;
        }
    }

    pce = bR;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq)) >= 0 && MIRROR64(SQ64(sq)) <= 63);
        score -= RookTable[MIRROR64(SQ64(sq))];
        ASSERT(fileRankValid(filesBrd[sq]));
        if(!(pos->pawns[BOTH] & fileBBMask[filesBrd[sq]])) {
            score -= RookOpenFile;
        } else if(!(pos->pawns[BLACK] & fileBBMask[filesBrd[sq]])) {
            score -= RookSemiOpenFile;
        }
    }

    pce = wQ;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        if(!(pos->pawns[BOTH] & fileBBMask[filesBrd[sq]])) {
            score += QueenOpenFile;
        } else if(!(pos->pawns[WHITE] & fileBBMask[filesBrd[sq]])) {
            score += QueenSemiOpenFile;
        }
    }

    pce = bQ;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        if(!(pos->pawns[BOTH] & fileBBMask[filesBrd[sq]])) {
            score -= QueenOpenFile;
        } else if(!(pos->pawns[BLACK] & fileBBMask[filesBrd[sq]])) {
            score -= QueenSemiOpenFile;
        }
    }

    pce = wK;
    sq = pos->pList[pce][0];
    ASSERT(sqOnBoard(sq));
    ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);

    if((pos->material[BLACK] <= ENDGAME_MAT)) {
        score += KingE[SQ64(sq)];
    } else {
        score += KingO[SQ64(sq)];
    }

    pce = bK;
    sq = pos->pList[pce][0];
    ASSERT(sqOnBoard(sq));
    ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);

    if((pos->material[WHITE] <= ENDGAME_MAT)) {
        score -= KingE[MIRROR64(SQ64(sq))];
    } else {
        score -= KingO[MIRROR64(SQ64(sq))];
    }

    if(pos->pceNum[wB] >= 2) score += BishopPair;
    if(pos->pceNum[bB] >= 2) score -= BishopPair;

    if(pos->side == WHITE) {
        return score;
    } else {
        return -score;
    }
}