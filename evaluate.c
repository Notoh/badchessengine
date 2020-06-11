#include "evaluate.h"
#include "psqt.h"
#include "board.h"
#include "validate.h"

const int PawnIsolated = S(-5, -12);
const int PawnPassed[8] = { S(0,0) , S(-9, 1), S(-16, 9), S(0, 30), S(25, 69), S(70, 105), S(110, 139), S(1000, 1000)};
const int RookOpenFile = S(46, 18);
const int RookSemiOpenFile = S(15, 19);
const int QueenOpenFile = S(-12, 15);
const int QueenSemiOpenFile = S(7, 9);
const int BishopPair = S(21, 68);
const int Tempo = 18;


const int pieceVal[13] = { 0, S(100, 113), S(434, 389), S(454, 407), S(629, 651), S(1239,1283), 0, S(100, 113), S(434, 389), S(454, 407), S(629, 651), S(1239,1283), 0 };



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

static int evalPawns(const S_BOARD *pos, int side) {
    int eval = 0;

    int piece = side == WHITE ? wP : bP;
    for(int i = 0; i < pos->pceNum[piece]; i++) {
        int sq = pos->pList[piece][i];

        if ((isolatedMask[SQ64(sq)] & pos->pawns[pos->side]) == 0) {
            eval += PawnIsolated;
        }

        if (side == WHITE) {
            if ((whitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
                eval += PawnPassed[ranksBrd[sq]];
            }
        } else {
            if ((blackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
                eval += PawnPassed[7 - ranksBrd[sq]];
            }
        }
    }

    return eval;
}

static int evalPiece(const S_BOARD *pos, int side, int pieceType) {

    int eval = 0;

    int bishop = side == WHITE ? wB : bB;
    int queen = side == WHITE ? wQ : bQ;
    int rook = side == WHITE ? wR : bR;

    if(pieceType == bishop && pos->pceNum[bishop] >= 2) {
        eval += BishopPair;
    }

    if(pieceType == queen) {
        for(int i = 0; i < pos->pceNum[pieceType]; i++) {
            int sq = pos->pList[pieceType][i];
            if(!(pos->pawns[BOTH] & fileBBMask[filesBrd[sq]])) {
                eval += QueenOpenFile;
            } else if(!(pos->pawns[side] & fileBBMask[filesBrd[sq]])) {
                eval += QueenSemiOpenFile;
            }
        }
    }

    if(pieceType == rook) {
        for(int i = 0; i < pos->pceNum[pieceType]; i++) {
            int sq = pos->pList[pieceType][i];
            if(!(pos->pawns[BOTH] & fileBBMask[filesBrd[sq]])) {
                eval += RookOpenFile;
            } else if(!(pos->pawns[side] & fileBBMask[filesBrd[sq]])) {
                eval += RookSemiOpenFile;
            }
        }
    }
    return eval;
}

static int evalKings(const S_BOARD *pos) {
    int eval = 0;

    int king = pos->side == WHITE ? wK : bK;

    //todo king danger
    int sq = pos->pList[king][0];

    return eval;
}

static int evalPieces(const S_BOARD *pos) {
    return evalPawns(pos, WHITE)
        - evalPawns(pos, BLACK)
        //no need for knights yet as they don't have anything except a psqt
        + evalPiece(pos, WHITE, wB)
        - evalPiece(pos, BLACK, bB)
        + evalPiece(pos, WHITE, wR)
        - evalPiece(pos, BLACK, bR)
        + evalPiece(pos, WHITE, wQ)
        - evalPiece(pos, BLACK, bQ);

}

int eval(const S_BOARD *pos) {
    ASSERT(checkBoard(pos));

    if(materialDraw(pos))
        return 0;

    int eval = pos->material[WHITE] - pos->material[BLACK];

    eval += evalPieces(pos);

    //interpolate eval by game phase, math from Ethereal
    int phase = 24 - 4 * (pos->pceNum[wQ] + pos->pceNum[bQ])
            - 2 * (pos->pceNum[wR] + pos->pceNum[bR])
            - 1 * (pos->pceNum[wN] + pos->pceNum[bN] + pos->pceNum[wB] + pos->pceNum[bB]);
    phase = (phase * 256 + 12) / 24;

    eval = ((ScoreMG(eval) * phase)
            +  (ScoreEG(eval) * (256 - phase)))
           / 256;

    //tempo after interp
    eval += pos->side == WHITE ? Tempo : -Tempo;

    return pos->side == WHITE ? eval : -eval;
}