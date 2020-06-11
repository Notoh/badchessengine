#include "init.h"
#include "uci.h"
#include "tt.h"
#include <stdlib.h>


int main(void) {

    init();
    S_BOARD board[1];
    S_SEARCHINFO info[1];
    info->quit = FALSE;
    board->hashtable->hashTable = NULL;
    initHashTable(board->hashtable, 64);

#ifndef CONSOLE
    uciLoop(board, info);
#else
    consoleLoop(board, info);
#endif
    free(board->hashtable->hashTable);

    return 0;

}
