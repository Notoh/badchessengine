#include "tt.h"
#include <stdlib.h>
#include <stdio.h>
#include "movegen.h"
#include "makemove.h"

int getPvLine(S_BOARD *pos, const int depth) {
    ASSERT(depth < MAXDEPTH);

    int move = probePvMove(pos);
    int count = 0;

    while(move != NOMOVE && count < depth) {
        ASSERT(count < MAXDEPTH);

        if(moveExists(pos, move)) {
            makeMove(pos, move);
            pos->pvarray[count++] = move;
        } else {
            break;
        }
        move = probePvMove(pos);
    }

    while(pos->ply > 0) {
        takeMove(pos);
    }

    return count;
}

void clearHashTable(S_HASHTABLE *table) {
    S_HASHENTRY *hashentry;

    for(hashentry = table->hashTable; hashentry < table->hashTable + table->numEntries; hashentry++) {
        hashentry->posKey = 0ULL;
        hashentry->move = NOMOVE;
        hashentry->depth = 0;
        hashentry->score = 0;
        hashentry->flags =0;
    }
    table->newWrite = 0;
}

void initHashTable(S_HASHTABLE *table, const int MB)  {
    int hashsize = 0x100000 * MB;
    table->numEntries = hashsize / sizeof(S_HASHENTRY);
    table->numEntries -= 2;

    if(table->hashTable != NULL) {
        free(table->hashTable);
    }

    table->hashTable = (S_HASHENTRY *) malloc(table->numEntries * sizeof(S_HASHENTRY));
    if(table->hashTable == NULL) {
        printf("Hash allocation failed\n");
        initHashTable(table, MB/2);
    } else {
        clearHashTable(table);
    }
}

void storeHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth) {
    int index = pos->posKey % pos->hashtable->numEntries;
    ASSERT(index >= 0 && index <= pos->pvtable->numEntries - 1);

    if(pos->hashtable->hashTable[index].posKey == 0) {
        pos->hashtable->newWrite++;
    } else {
        pos->hashtable->overwrite++;
    }

    if(score > ISMATE) score += pos->ply;
    else if(score < -ISMATE) score -= pos->ply;

    pos->hashtable->hashTable[index].move = move;
    pos->hashtable->hashTable[index].posKey = pos->posKey;
    pos->hashtable->hashTable[index].flags = flags;
    pos->hashtable->hashTable[index].score = score;
    pos->hashtable->hashTable[index].depth = depth;
}

int probeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth) {
    int index = pos->posKey % pos->hashtable->numEntries;

    ASSERT(index >= 0 && index <= pos->pvtable->numEntries - 1);
    ASSERT(depth >= 1 && depth < MAXDEPTH);
    ASSERT(alpha<beta);
    ASSERT(alpha>=-INFINITE&&alpha<=INFINITE);
    ASSERT(beta>=-INFINITE&&beta<=INFINITE);
    ASSERT(pos->ply>=0 && pos->ply < MAXDEPTH);

    if(pos->hashtable->hashTable[index].posKey == pos->posKey) {
        *move = pos->hashtable->hashTable[index].move;

        if(pos->hashtable->hashTable[index].depth >= depth) {
            pos->hashtable->hit++;
            ASSERT(pos->hashtable->hashTable[index].depth>=1&&pos->hashtable->hashTable[index].depth<MAXDEPTH);
            ASSERT(pos->hashtable->hashTable[index].flags>=HFALPHA&&pos->hashtable->hashTable[index].flags<=HFEXACT);

            *score =pos->hashtable->hashTable[index].score;
            if(*score > ISMATE) *score -= pos->ply;
            else if(*score < -ISMATE) *score += pos->ply;

            switch(pos->hashtable->hashTable[index].flags) {
                ASSERT(*score==pos->hashtable->hashTable[index].score);
                ASSERT(*score>=-INFINITE&&*score<=INFINITE);

                case HFALPHA:
                    if(*score<=alpha) {
                        *score = alpha;
                        return TRUE;
                    }
                    break;
                case HFBETA:
                    if(*score>=beta) {
                        *score = beta;
                        return TRUE;
                    }
                    break;
                case HFEXACT:
                    return TRUE;
                    break;
                default: ASSERT(FALSE); break;
            }
        }
    }

    return FALSE;
}

int probePvMove(const S_BOARD *pos) {

    int index = pos->posKey % pos->hashtable->numEntries;
    ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);

    if(pos->hashtable->hashTable[index].posKey == pos->posKey) {
        return pos->hashtable->hashTable[index].move;
    }

    return NOMOVE;
}

//based on weiss
int hashFull(const S_BOARD *pos) {

    int used = 0;
    const int samples = 1000;
    for(int i = 0; i < samples; i++) {
        if (pos->hashtable->hashTable[i].move != NOMOVE)
            used++;
    }
    return used / (samples / 1000);
}