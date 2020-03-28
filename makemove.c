#include "defs.h"

#define HASH_PCE(pce,sq) (pos->posKey ^= (pieceKeys[(pce)][(sq)]))
#define HASH_CA (pos->posKey ^= (castleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= (sideKey))
#define HASH_EP (pos->posKey ^= ([pieceKeys[EMPTY][(pos->enPas)]))

const int castlePerm[120] = {
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

static void clearPiece(S_BOARD *pos, const int sq) {
    ASSERT(sqOnBoard(sq));

    int pce = pos->pieces[sq];

    ASSERT(pieceValid(pce));

    int col = pieceCol[pce];
    int index = 0;
    int t_pceNum = -1;

    HASH_PCE(pce, sq);

    pos->pieces[sq] = EMPTY;
    pos->material[col] -= pieceVal[pce];

    if(pieceBig[pce]) {
        pos->bigPce[col]--;
        if(pieceMin[pce] != 0) {
            pos->minPce[col]--;
        } else {
            pos->majPce[col]--;
        }
    } else {
        CLRBIT(pos->pawns[col], SQ64(sq));
        CLRBIT(pos->pawns[BOTH], SQ64(sq));
    }

    for(index = 0; index < pos->pceNum[pce]; index++) {
        if(pos->pList[pce][index] == sq) {
            t_pceNum = index;
            break;
        }
    }

    ASSERT(t_pceNum != -1);
    //we have 1 less piece, find the square we are clearing and then updates this square's spot in the array with the last piece
    pos->pceNum[pce]--;
    pos->pList[pce][t_pceNum] = pos->pList[pce][pos->pceNum[pce]];
}

static void addPiece(S_BOARD *pos, const int sq, const int pce) {
    ASSERT(pieceValid(pce));
    ASSERT(sqOnBoard(sq));

    int col = pieceCol[pce];

    HASH_PCE(pce,sq);
    pos->pieces[sq] = pce;

    if(pieceBig[pce]) {
        pos->bigPce[col]++;
        if(pieceMin[pce] != 0) {
            pos->minPce[col]++;
        } else {
            pos->majPce[col]++;
        }
    } else {
        SETBIT(pos->pawns[col], SQ64(sq));
        SETBIT(pos->pawns[BOTH], SQ64(sq));
    }

    pos->material[col] += pieceVal[pce];
    pos->pList[pce][pos->pceNum[pce]++] = sq;
}

static void movePiece(S_BOARD *pos, const int from, const int to) {
    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));

    int index = 0;
    int pce = pos->pieces[from];
    int col = pieceCol[pce];
#ifdef DEBUG
    int t_PieceNum = FALSE;
#endif
    HASH_PCE(pce,from);
    pos->pieces[from] = EMPTY;

    HASH_PCE(pce, to);
    pos->pieces[to] = pce;

    if(!pieceBig[pce]) {
        CLRBIT(pos->pawns[col], SQ64(from));
        CLRBIT(pos->pawns[BOTH], SQ64(from));
        SETBIT(pos->pawns[col], SQ64(to));
        SETBIT(pos->pawns[BOTH], SQ64(to));
    }

    for(index = 0; index < pos->pceNum[pce]; index++) {
        if(pos->pList[pce][index] == from) {
            pos->pList[pce][index] = to;
#ifdef DEBUG
            t_PieceNum = TRUE;
#endif
            break;
        }
    }
    ASSERT(t_PieceNum);
}