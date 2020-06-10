//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_UCI_H
#define BADCHESSENGINE_UCI_H

//#define CONSOLE

#include "types.h"


void uciLoop(S_BOARD *pos, S_SEARCHINFO *info);
void consoleLoop(S_BOARD *pos, S_SEARCHINFO *info);

#endif //BADCHESSENGINE_UCI_H
