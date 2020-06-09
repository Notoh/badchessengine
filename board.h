#ifndef BADCHESSENGINE_BOARD_H
#define BADCHESSENGINE_BOARD_H

#include <stdio.h>
#include "types.h"
#include "bitboards.h"

void resetBoard(S_BOARD *pos);
int parseFEN(char *fen, S_BOARD *pos);
void printBoard(const S_BOARD *pos);
void updateListsMaterial(S_BOARD *pos);
int checkBoard(const S_BOARD *pos);
void mirrorBoard(S_BOARD *pos);

#endif //BADCHESSENGINE_BOARD_H
