
#include "defs.h"

#define RAND_64  ( (u64)rand() | \
                   (u64)rand() << 15 | \
                   (u64)rand() << 30 | \
                   (u64)rand() << 45 | \
                   ((u64)rand() & 0xf) << 60)

int sq120tosq64[BRD_SQ_NUM];
int sq64tosq120[64];
u64 setMask[64];
u64 clearMask[64];

u64 pieceKeys[13][120];
u64 sideKey;
u64 castleKeys[16];

int filesBrd[BRD_SQ_NUM];
int ranksBrd[BRD_SQ_NUM];

void initFilesRanksBrd() {
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;

    for(index = 0; index < BRD_SQ_NUM; index++) {
        filesBrd[index] = OFFBOARD;
        ranksBrd[index] = OFFBOARD;
    }
    for(rank = RANK_1; rank <= RANK_8; rank++) {
        for(file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file,rank);
            filesBrd[sq] = file;
            ranksBrd[sq] = rank;
        }
    }

}

void initHashKeys() {
    int index = 0;
    int index2 = 0;
    for(index = 0; index < 13; index++) {
        for(index2 = 0; index2 < 120; index2++) {
            pieceKeys[index][index2] = RAND_64;
        }
    }
    sideKey = RAND_64;
    for(index = 0; index < 16; index++) {
        castleKeys[index] = RAND_64;
    }
}

void initBitMasks() {
    int index = 0;

    for(index = 0; index < 64; index++) {
        setMask[index] = 0ULL;
        clearMask[index] = 0ULL;
    }

    for(index = 0; index < 64; index++) {
        setMask[index] |= (1ULL << index);
        clearMask[index] = ~setMask[index];
    }
}

void initSq120toSq64() {
     int index = 0;
     int file = FILE_A;
     int rank = RANK_1;
     int sq = A1;
     int sq64 = 0;
     for(index = 0; index < BRD_SQ_NUM; index++) {
         sq120tosq64[index] = 65;
     }
     for(index = 0; index < 64; index++) {
         sq64tosq120[index] = 120;
     }

     for(rank = RANK_1; rank <= RANK_8; rank++) {
         for(file = FILE_A; file <= FILE_H; file++) {
             sq = FR2SQ(file,rank);
             sq64tosq120[sq64] = sq;
             sq120tosq64[sq] = sq64;
             sq64++;
         }
     }
}

void init() {
    initSq120toSq64();
    initBitMasks();
    initHashKeys();
    initFilesRanksBrd();
}



