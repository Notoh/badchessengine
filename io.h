//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_IO_H
#define BADCHESSENGINE_IO_H

#include "types.h"

char *prmove(const int move);
char *prsq(const int sq);
void printmvlist(const S_MOVELIST *list);
int parsemove(const char *ptrchar, S_BOARD *pos);


#endif //BADCHESSENGINE_IO_H
