
#include "defs.h"

int sq120tosq64[BRD_SQ_NUM];
int sq64tosq120[64];
u64 setMask[64];
u64 clearMask[64];

u64 pieceKeys[13][120];
u64 sideKey;
u64 castleKeys[16];

int filesBrd[BRD_SQ_NUM];
int ranksBrd[BRD_SQ_NUM];

u64 fileBBMask[8];
u64 rankBBMask[8];

u64 blackPassedMask[64];
u64 whitePassedMask[64];
u64 isolatedMask[64];

static void initEvalMasks() {
    int sq, tsq, r, f;

    for(sq = 0; sq < 8; ++sq) {
        fileBBMask[sq] = 0ULL;
        rankBBMask[sq] = 0ULL;
    }

    for(r = RANK_8; r >= RANK_1; r--) {
        for (f = FILE_A; f <= FILE_H; f++) {
            sq = r * 8 + f;
            fileBBMask[f] |= (1ULL << sq);
            rankBBMask[r] |= (1ULL << sq);
        }
    }

    for(sq = 0; sq < 64; ++sq) {
        isolatedMask[sq] = 0ULL;
        whitePassedMask[sq] = 0ULL;
        blackPassedMask[sq] = 0ULL;
    }

    for(sq = 0; sq < 64; ++sq) {
        tsq = sq + 8;

        while(tsq < 64) {
            whitePassedMask[sq] |= (1ULL << tsq);
            tsq += 8;
        }

        tsq = sq - 8;
        while(tsq >= 0) {
            blackPassedMask[sq] |= (1ULL << tsq);
            tsq -= 8;
        }

        if(filesBrd[SQ120(sq)] > FILE_A) {
            isolatedMask[sq] |= fileBBMask[filesBrd[SQ120(sq)] - 1];

            tsq = sq + 7;
            while(tsq < 64) {
                whitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 9;
            while(tsq >= 0) {
                blackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }

        if(filesBrd[SQ120(sq)] < FILE_H) {
            isolatedMask[sq] |= fileBBMask[filesBrd[SQ120(sq)] + 1];

            tsq = sq + 9;
            while(tsq < 64) {
                whitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 7;
            while(tsq >= 0) {
                blackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }
    }
}

static void initFilesRanksBrd() {
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

static u64 rand64() {

    // http://vigna.di.unimi.it/ftp/papers/xorshift.pdf

    static u64 seed = 1070372ull;

    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;

    return seed * 2685821657736338717ull;

}

static void initHashKeys() {
    int index = 0;
    int index2 = 0;
    for(index = 0; index < 13; index++) {
        for(index2 = 0; index2 < 120; index2++) {
            pieceKeys[index][index2] = rand64();
        }
    }
    sideKey = rand64();
    for(index = 0; index < 16; index++) {
        castleKeys[index] = rand64();
    }
}

static void initBitMasks() {
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

static void initSq120toSq64() {
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
    initEvalMasks();
    initMvvLva();
}



