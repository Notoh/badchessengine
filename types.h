//
// Created by ADMIN on 6/9/2020.
//

#ifndef BADCHESSENGINE_TYPES_H
#define BADCHESSENGINE_TYPES_H

#include <stdint.h>

#define NAME "Bad Chess Engine 0.4"

#define BRD_SQ_NUM 120
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define IsBQ(p) (pieceBishopQueen[(p)])
#define IsRQ(p) (pieceRookQueen[(p)])
#define IsKn(p) (pieceKnight[(p)])
#define IsKi(p) (pieceKing[(p)])
#define MAXDEPTH 100
#define INFINITE 30000
#define ISMATE (INFINITE - MAXDEPTH)
#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVES 256
#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000
#define MFLAGCAP 0x7c000
#define MFLAGPROM 0xf00000

#define NOMOVE 0

#define MOVE(f,t,ca,pro,fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))

//#define DEBUG


#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s- Failed",#n); \
printf("On %s ",__DATE__); \
printf("At %s ",__TIME__); \
printf("In File %s ",__FILE__); \
printf("At Line %d\n",__LINE__); \
exit(1);}
#endif
#define FROMSQ(m) ((m) & 0x7f)
#define TOSQ(m) (((m)>>7) & 0x7f)
#define CAPTURED(m) (((m)>>14) & 0xf)
#define PROMOTED(m) (((m)>>20) & 0xf)
#define FR2SQ(f,r) ((21 + (f)) + ((r) * 10))
#define SQ64(sq120) (sq120tosq64[sq120])
#define SQ120(sq64) (sq64tosq120[sq64])
#define SQOFFBOARD(sq) (filesBrd[(sq)]==OFFBOARD)
#define MIRROR64(sq) (Mirror64[(sq)])

typedef struct {
    uint64_t posKey;
    int move;
    int score;
    int depth;
    int flags;
} S_HASHENTRY;

typedef struct {
    S_HASHENTRY *hashTable;
    int numEntries;
    int newWrite;
    int overwrite;
    int hit;
    int cut;
} S_HASHTABLE;

typedef struct {
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    uint64_t posKey;

} S_UNDO;

typedef struct Board {
    int pieces[BRD_SQ_NUM];
    uint64_t pawns[3]; //bit structure of if pawns are here or not

    int kingSq[2];

    int side;
    int enPas;
    int fiftyMove;

    int ply;
    int histPly;

    int castlePerm;

    uint64_t posKey;

    int pceNum[13]; //how many pieces, there are 13 values because 12 pcs + none
    int bigPce[2]; //not pawns
    int majPce[2]; //rooks + queen
    int minPce[2]; //bishops + knights
    int material[2];

    S_UNDO history[MAXGAMEMOVES];

    //piece list
    int pList[13][10];

    S_HASHTABLE hashtable[1];
    int pvarray[MAXDEPTH];

    int searchHistory[13][BRD_SQ_NUM];
    int searchKillers[2][MAXDEPTH];

} S_BOARD;



typedef struct {
    long start;
    long stop;
    int depth;
    int depthset;
    int movestogo;
    int timeset;
    int nullCut;

    long nodes;

    int quit;
    int stopped;

    float fh;
    float fhf;

    int GAME_MODE;
    int POST_THINKING;

} S_SEARCHINFO;

typedef struct {
    int move;
    /* Move is in format
     * Castle (1 bit) 0x1000000
     * Promoted Piece (4 bits) >>20 0xf
     * pawn start (1 bit) 0x80000
     * enpassant (1 bit) 0x40000
     * captured piece (4 bits) >> 14, 0xf
     * to square (7 bits) >> 7 0x7f
     * from square (7 bits) 0x7f <--- LSB
    */
    int score;

} S_MOVE;

typedef struct {
    S_MOVE moves[MAXPOSITIONMOVES];
    int count;
} S_MOVELIST;


enum { FALSE, TRUE }; //i'm lazy
enum { WHITE, BLACK, BOTH };

const char* pceChar;
const char* sideChar;
const char* rankChar;
const char* fileChar;

enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
}; //square values

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK }; //piece values
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

//coordinate values


// bit structure of castle permissions or not, i.e. 1001 means white can castle kingside and black can castle queenside
enum { WSCA = 1, WLCA = 2, BSCA = 4, BLCA = 8};

enum { HFNONE, HFALPHA, HFBETA, HFEXACT };



int sq120tosq64[BRD_SQ_NUM];
int sq64tosq120[64];
uint64_t setMask[64];
uint64_t clearMask[64];
uint64_t pieceKeys[13][120];
uint64_t sideKey;
uint64_t castleKeys[16];
int filesBrd[BRD_SQ_NUM];
int ranksBrd[BRD_SQ_NUM];
uint64_t fileBBMask[8];
uint64_t rankBBMask[8];
uint64_t blackPassedMask[64];
uint64_t whitePassedMask[64];
uint64_t isolatedMask[64];
const int pieceVal[13];

const int pieceBig[13];
const int pieceMaj[13];
const int pieceMin[13];
const int pieceCol[13];
const int piecePawn[13];
const int pieceKnight[13];
const int pieceKing[13];
const int pieceRookQueen[13];
const int pieceBishopQueen[13];
const int pieceSlides[13];
const int Mirror64[64];

#endif //BADCHESSENGINE_TYPES_H
