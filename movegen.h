//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_MOVEGEN_H
#define BADCHESSENGINE_MOVEGEN_H

#include "types.h"

void generateAllMoves(const S_BOARD *pos, S_MOVELIST *movelist);
int moveExists(S_BOARD *pos, const int move);
void initMvvLva();
void generateAllCaps(const S_BOARD *pos, S_MOVELIST *movelist);


#endif //BADCHESSENGINE_MOVEGEN_H
