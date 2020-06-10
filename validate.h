#ifndef BADCHESSENGINE_VALIDATE_H
#define BADCHESSENGINE_VALIDATE_H

#include "types.h"

int sqOnBoard(const int sq);
int sideValid(const int side);
int fileRankValid(const int fr);
int pieceValidEmpty(const int pce);
int pieceValid(const int pce);
void mirrorEvalTest(S_BOARD *pos);
int sqIs120(const int sq);
int pceValidEmptyOffbrd(const int pce);

#endif //BADCHESSENGINE_VALIDATE_H
