//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_TT_H
#define BADCHESSENGINE_TT_H

#include "types.h"

void initHashTable(S_HASHTABLE *table, const int MB);
void storeHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);
int probeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);
int getPvLine(S_BOARD *pos, const int depth);
void clearHashTable(S_HASHTABLE *table);
int probePvMove(const S_BOARD *pos);


#endif //BADCHESSENGINE_TT_H
