//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_BITBOARDS_H
#define BADCHESSENGINE_BITBOARDS_H

#include <stdio.h>
#include "types.h"

#define POP(b) popBit(b)
#define CNT(b) countBits(b)
#define CLRBIT(bb,sq) ((bb) &= clearMask[(sq)])
#define SETBIT(bb,sq) ((bb) |= setMask[(sq)])

void printBitboard(uint64_t bb);
int popBit(uint64_t *bb);
int countBits(uint64_t b);


#endif //BADCHESSENGINE_BITBOARDS_H
