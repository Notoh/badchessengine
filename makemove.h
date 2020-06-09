//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_MAKEMOVE_H
#define BADCHESSENGINE_MAKEMOVE_H

#include "types.h"
#include "attack.h"
#include "bitboards.h"

int makeMove(S_BOARD *pos, int move);
void takeMove(S_BOARD *pos);
void makeNullMove(S_BOARD *pos);
void takeNullMove(S_BOARD *pos);


#endif //BADCHESSENGINE_MAKEMOVE_H
