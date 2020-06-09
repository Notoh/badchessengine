//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_UCI_H
#define BADCHESSENGINE_UCI_H

//#define CONSOLE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "board.h"
#include "validate.h"
#include "util.h"
#include "search.h"

void uciLoop(S_BOARD *pos, S_SEARCHINFO *info);
void consoleLoop(S_BOARD *pos, S_SEARCHINFO *info);

#endif //BADCHESSENGINE_UCI_H
