#include "defs.h"

const int pvsize = 0x100000 * 2; //TODO make this adjustable

int getPvLine(S_BOARD *pos, const int depth) {
    ASSERT(depth < MAXDEPTH);

    int move = probePvTable(pos);
    int count = 0;

    while(move != NOMOVE && count < depth) {
        ASSERT(count < MAXDEPTH);

        if(moveExists(pos, move)) {
            makeMove(pos, move);
            pos->pvarray[count++] = move;
        } else {
            break;
        }
        move = probePvTable(pos);
    }

    while(pos->ply > 0) {
        takeMove(pos);
    }

    return count;
}

void clearPvTable(S_PVTABLE *table) {
    S_PVENTRY *pventry;

    for(pventry = table->pTable; pventry < table->pTable + table->numEntries; pventry++) {
        pventry->posKey = 0ULL;
        pventry->move = NOMOVE;
    }
}

void initPvTable(S_PVTABLE *table)  {
    table->numEntries = pvsize / sizeof(S_PVENTRY);
    table->numEntries -= 2;

    if(table->pTable != NULL) {
        free(table->pTable);
    }

    table->pTable = (S_PVENTRY *) malloc(table->numEntries * sizeof(S_PVENTRY));
    clearPvTable(table);

}

void storePvMove(const S_BOARD *pos, const int move) {
    int index = pos->posKey % pos->pvtable->numEntries;
    ASSERT(index >= 0 && index <= pos->pvtable->numEntries - 1);

    pos->pvtable->pTable[index].move = move;
    pos->pvtable->pTable[index].posKey = pos->posKey;
}

int probePvTable(const S_BOARD *pos) {
    int index = pos->posKey % pos->pvtable->numEntries;

    ASSERT(index >= 0 && index <= pos->pvtable->numEntries - 1);
    if(pos->pvtable->pTable[index].posKey == pos->posKey) {
        return pos->pvtable->pTable[index].move;
    }

    return NOMOVE;
}