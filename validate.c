#include "defs.h"

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