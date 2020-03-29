#include "defs.h"

void searcHPosition(S_BOARD *pos) {

}

int isRepetition(const S_BOARD *pos) {
    int index = 0;

    for(index = pos->histPly - pos->fiftyMove; index<pos->histPly-1; index++) {
        if(pos->posKey == pos->history[index].posKey) {
            return TRUE;
        }
    }
    return FALSE;
}
