#ifndef BADCHESSENGINE_VALIDATE_H
#define BADCHESSENGINE_VALIDATE_H

extern int sqOnBoard(const int sq);
extern int sideValid(const int side);
extern int fileRankValid(const int fr);
extern int pieceValidEmpty(const int pce);
extern int pieceValid(const int pce);
extern void mirrorEvalTest(S_BOARD *pos);
extern int sqIs120(const int sq);
extern int pceValidEmptyOffbrd(const int pce);

#endif //BADCHESSENGINE_VALIDATE_H
