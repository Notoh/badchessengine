//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_EVALUATE_H
#define BADCHESSENGINE_EVALUATE_H

#include "types.h"

#define MakeScore(mg, eg) ((int)((unsigned int)(eg) << 16) + (mg))
#define S(mg, eg) MakeScore(mg, eg)
#define ScoreMG(s) ((int16_t)((uint16_t)((unsigned)((s)))))
#define ScoreEG(s) ((int16_t)((uint16_t)((unsigned)((s) + 0x8000) >> 16)))

int eval(const S_BOARD *pos);

#endif //BADCHESSENGINE_EVALUATE_H
