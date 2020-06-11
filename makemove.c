#include "makemove.h"
#include "attack.h"
#include "bitboards.h"
#include "psqt.h"
#include "validate.h"
#include "board.h"


#define HASH_PCE(pce,sq) (pos->posKey ^= (pieceKeys[(pce)][(sq)]))
#define HASH_CA (pos->posKey ^= (castleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= (sideKey))
#define HASH_EP (pos->posKey ^= (pieceKeys[EMPTY][pos->enPas]))
#define INCHECK (sqAttacked(pos->kingSq[pos->side], pos->side^1, pos))

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
    pos->material[col] -= PSQT[pce][SQ64(sq)];

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

    pos->material[col] += PSQT[pce][SQ64(sq)];
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

int makeMove(S_BOARD *pos, int move) {
    ASSERT(checkBoard(pos));

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;

    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));
    ASSERT(sideValid(side));
    ASSERT(pieceValid(pos->pieces[from]));

    pos->history[pos->histPly].posKey = pos->posKey;

    if(move & MFLAGEP) {
        clearPiece(pos, side == WHITE ? to - 10 : to + 10);
    } else if(move & MFLAGCA) {
        switch(to) {
            case C1:
                movePiece(pos,A1,D1);
                break;
            case C8:
                movePiece(pos, A8, D8);
                break;
            case G1:
                movePiece(pos, H1, F1);
                break;
            case G8:
                movePiece(pos, H8, F8);
                break;
            default:
                ASSERT(FALSE); break;
        }
    }

    if(pos->enPas != NO_SQ) {
        HASH_EP;
    }
    HASH_CA;

    pos->history[pos->histPly].move = move;
    pos->history[pos->histPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->histPly].enPas = pos->enPas;
    pos->history[pos->histPly].castlePerm = pos->castlePerm;

    pos->castlePerm &= castlePerm[from];
    pos->castlePerm &= castlePerm[to];

    pos->enPas = NO_SQ;

    HASH_CA;

    int captured = CAPTURED(move);
    pos->fiftyMove++;

    if(captured != EMPTY) {
        ASSERT(pieceValid(captured));
        clearPiece(pos, to);
        pos->fiftyMove = 0;
    }

    pos->histPly++;
    pos->ply++;

    if(piecePawn[pos->pieces[from]]) {
        pos->fiftyMove = 0;
        if(move & MFLAGPS) {
            if(side == WHITE) {
                pos->enPas=from+10;
                ASSERT(ranksBrd[pos->enPas] == RANK_3);
            } else {
                pos->enPas=from-10;
                ASSERT(ranksBrd[pos->enPas] == RANK_6);
            }
            HASH_EP;
        }
    }

    movePiece(pos,from,to);

    int prPce = PROMOTED(move);
    if(prPce != EMPTY) {
        ASSERT(pieceValid(prPce) && !piecePawn[prPce]);
        clearPiece(pos, to);
        addPiece(pos, to, prPce);
    }

    if(pieceKing[pos->pieces[to]]) {
        pos->kingSq[pos->side] = to;
    }

    pos->side ^= 1;

    HASH_SIDE;

    ASSERT(checkBoard(pos));

    if(sqAttacked(pos->kingSq[side], pos->side, pos)) {
        takeMove(pos);
        return FALSE;
    }

    return TRUE;


}

void takeMove(S_BOARD *pos) {
    ASSERT(checkBoard(pos));

    pos->histPly--;
    pos->ply--;

    ASSERT(pos->histPly >= 0 && pos->histPly < MAXGAMEMOVES);
    ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);

    int move = pos->history[pos->histPly].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);

    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));

    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->castlePerm = pos->history[pos->histPly].castlePerm;
    pos->fiftyMove = pos->history[pos->histPly].fiftyMove;
    pos->enPas = pos->history[pos->histPly].enPas;

    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->side ^= 1;
    HASH_SIDE;

    if(MFLAGEP & move) {
        if(pos->side == WHITE) {
            addPiece(pos, to-10, bP);
        } else {
            addPiece(pos, to+10, wP);
        }
    } else if(MFLAGCA & move) {
        switch(to) {
            case C1: movePiece(pos, D1, A1);
                break;
            case C8: movePiece(pos, D8, A8);
                break;
            case G1: movePiece(pos, F1, H1);
                break;
            case G8: movePiece(pos, F8, H8);
                break;
            default: ASSERT(FALSE);
                break;
        }
    }

    movePiece(pos, to, from);

    if(pieceKing[pos->pieces[from]]) {
        pos->kingSq[pos->side] = from;
    }

    int captured = CAPTURED(move);
    if(captured != EMPTY) {
        ASSERT(pieceValid(captured));
        addPiece(pos, to, captured);
    }

    if(PROMOTED(move) != EMPTY) {
        ASSERT(pieceValid(PROMOTED(move)) && !piecePawn[PROMOTED(move)]);
        clearPiece(pos, from);
        addPiece(pos, from, (pieceCol[PROMOTED(move)] == WHITE ? wP : bP));
    }
    ASSERT(checkBoard(pos));

}

void makeNullMove(S_BOARD *pos) {
    ASSERT(checkBoard(pos));
    ASSERT(!INCHECK);

    pos->ply++;
    pos->history[pos->histPly].posKey = pos->posKey;

    if(pos->enPas != NO_SQ) HASH_EP;

    pos->history[pos->histPly].move = NOMOVE;
    pos->history[pos->histPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->histPly].enPas = pos->enPas;
    pos->history[pos->histPly].castlePerm = pos->castlePerm;
    pos->enPas = NO_SQ;

    pos->side ^= 1;
    pos->histPly++;
    HASH_SIDE;

    ASSERT(checkBoard(pos));
}

void takeNullMove(S_BOARD *pos) {
    ASSERT(checkBoard(pos));

    pos->histPly--;
    pos->ply--;
    if(pos-> enPas != NO_SQ) HASH_EP;

    pos->castlePerm = pos->history[pos->histPly].castlePerm;
    pos->fiftyMove = pos->history[pos->histPly].fiftyMove;
    pos->enPas = pos->history[pos->histPly].enPas;

    if(pos->enPas != NO_SQ) HASH_EP;

    pos->side ^= 1;
    HASH_SIDE;
    ASSERT(checkBoard(pos));
}
