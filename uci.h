//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_UCI_H
#define BADCHESSENGINE_UCI_H

//#define CONSOLE

#include "types.h"


void uciLoop(S_BOARD *pos, S_SEARCHINFO *info);
void consoleLoop(S_BOARD *pos, S_SEARCHINFO *info);
inline int mateScore(const int score);
void printThinking(const S_BOARD  *pos, int depth, int score, long nodes, long elapsed, int pvMoves);
void printConclusion(const S_BOARD *pos, int bestMove);

#endif //BADCHESSENGINE_UCI_H
