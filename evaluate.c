#include "defs.h"

const int PawnIsolated = -10;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 };
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;

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
        0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
        -30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
        -50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
        -70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70
};
const int Mirror64[64] = {
        56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
        48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
        40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
        32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
        24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
        16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
        8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
        0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
};

#define MIRROR64(sq) (Mirror64[(sq)])

int materialDraw(const S_BOARD *pos) {

    ASSERT(checkBoard(pos));

    if (!pos->pceNum[wR] && !pos->pceNum[bR] && !pos->pceNum[wQ] && !pos->pceNum[bQ]) {
        if (!pos->pceNum[bB] && !pos->pceNum[wB]) {
            if (pos->pceNum[wN] < 3 && pos->pceNum[bN] < 3) {  return TRUE; }
        } else if (!pos->pceNum[wN] && !pos->pceNum[bN]) {
            if (abs(pos->pceNum[wB] - pos->pceNum[bB]) < 2) { return TRUE; }
        } else if ((pos->pceNum[wN] < 3 && !pos->pceNum[wB]) || (pos->pceNum[wB] == 1 && !pos->pceNum[wN])) {
            if ((pos->pceNum[bN] < 3 && !pos->pceNum[bB]) || (pos->pceNum[bB] == 1 && !pos->pceNum[bN]))  { return TRUE; }
        }
    } else if (!pos->pceNum[wQ] && !pos->pceNum[bQ]) {
        if (pos->pceNum[wR] == 1 && pos->pceNum[bR] == 1) {
            if ((pos->pceNum[wN] + pos->pceNum[wB]) < 2 && (pos->pceNum[bN] + pos->pceNum[bB]) < 2)	{ return TRUE; }
        } else if (pos->pceNum[wR] == 1 && !pos->pceNum[bR]) {
            if ((pos->pceNum[wN] + pos->pceNum[wB] == 0) && (((pos->pceNum[bN] + pos->pceNum[bB]) == 1) || ((pos->pceNum[bN] + pos->pceNum[bB]) == 2))) { return TRUE; }
        } else if (pos->pceNum[bR] == 1 && !pos->pceNum[wR]) {
            if ((pos->pceNum[bN] + pos->pceNum[bB] == 0) && (((pos->pceNum[wN] + pos->pceNum[wB]) == 1) || ((pos->pceNum[wN] + pos->pceNum[wB]) == 2))) { return TRUE; }
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
    }

    pce = bP;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq)) >= 0 && MIRROR64(SQ64(sq)) <= 63);
        score -= PawnTable[MIRROR64(SQ64(sq))];
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
    }
    pce = bR;
    for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
        sq = pos->pList[pce][pceNum];
        ASSERT(sqOnBoard(sq));
        ASSERT(MIRROR64(SQ64(sq)) >= 0 && MIRROR64(SQ64(sq)) <= 63);
        score -= RookTable[MIRROR64(SQ64(sq))];
        ASSERT(fileRankValid(filesBrd[sq]));
    }
    pce = wK;
    sq = pos->pList[pce][0];
    ASSERT(sqOnBoard(sq));
    ASSERT(SQ64(sq)>=0 && SQ64(sq)<=63);

    if( (pos->material[BLACK] <= ENDGAME_MAT) ) {
        score += KingE[SQ64(sq)];
    } else {
        score += KingO[SQ64(sq)];
    }

    pce = bK;
    sq = pos->pList[pce][0];
    ASSERT(sqOnBoard(sq));
    ASSERT(MIRROR64(SQ64(sq))>=0 && MIRROR64(SQ64(sq))<=63);

    if( (pos->material[WHITE] <= ENDGAME_MAT) ) {
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