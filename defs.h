
#ifndef BCE_DEFS_H
#define BCE_DEFS_H

#include <stdlib.h>
#include <stdio.h>

#define DEBUG

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

typedef unsigned long long u64;

/* Engine Info */

#define NAME "Bad Chess Engine 1.0"

#define BRD_SQ_NUM 120

#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 128

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK }; //piece values
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
//coordinate values
enum { WHITE, BLACK, BOTH };

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

enum { FALSE, TRUE }; //i'm lazy


// bit structure of castle permissions or not, i.e. 1001 means white can castle kingside and black can castle queenside
enum { WSCA = 1, WLCA = 2, BSCA = 4, BLCA = 8};

typedef struct {
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    u64 posKey;


} S_UNDO;


typedef struct {
    int pieces[BRD_SQ_NUM];
    u64 pawns[3]; //bit structure of if pawns are here or not

    int kingSq[2];

    int side;
    int enPas;
    int fiftyMove;

    int ply;
    int histPly;

    int castlePerm;

    u64 posKey;

    int pceNum[13]; //how many pieces, there are 13 values because 12 pcs + none
    int bigPce[2]; //not pawns
    int majPce[2]; //rooks + queen
    int minPce[2]; //bishops + knights
    int material[2];

    S_UNDO history[MAXGAMEMOVES];

    //piece list
    int pList[13][10];


} S_BOARD;

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


#define FROMSQ(m) ((m) & 0x7f)
#define TOSQ(m) (((m)>>7) & 0x7f)
#define CAPTURED(m) (((m)>>14) & 0xf)
#define PROMOTED(m) (((m)>>20) & 0xf)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000
#define MFLAGCAP 0x7c000
#define MFLAGPROM 0xf00000

#define NOMOVE 0

/* macros */

#define FR2SQ(f,r) ((21 + (f)) + ((r) * 10))
#define SQ64(sq120) (sq120tosq64[sq120])
#define SQ120(sq64) (sq64tosq120[sq64])
#define POP(b) popBit(b)
#define CNT(b) countBits(b)
#define CLRBIT(bb,sq) ((bb) &= clearMask[(sq)])
#define SETBIT(bb,sq) ((bb) |= setMask[(sq)])
#define IsBQ(p) (pieceBishopQueen[(p)])
#define IsRQ(p) (pieceRookQueen[(p)])
#define IsKn(p) (pieceKnight[(p)])
#define IsKi(p) (pieceKing[(p)])
#define MOVE(f,t,ca,pro,fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))
#define SQOFFBOARD(sq) (filesBrd[(sq)]==OFFBOARD)

/* Globals */

extern int sq120tosq64[BRD_SQ_NUM];
extern int sq64tosq120[64];
extern u64 setMask[64];
extern u64 clearMask[64];
extern u64 pieceKeys[13][120];
extern u64 sideKey;
extern u64 castleKeys[16];
extern char pceChar[];
extern char sideChar[];
extern char rankChar[];
extern char fileChar[];

extern int pieceBig[13];
extern int pieceMaj[13];
extern int pieceMin[13];
extern int pieceVal[13];
extern int pieceCol[13];

extern int filesBrd[BRD_SQ_NUM];
extern int ranksBrd[BRD_SQ_NUM];

extern int pieceKnight[13];
extern int pieceKing[13];
extern int pieceRookQueen[13];
extern int pieceBishopQueen[13];
extern int pieceSlides[13];
extern int piecePawn[13];

/* Functions */
//hashkeys.c

extern u64 generatePosKey(const S_BOARD *pos);

//board.c

extern void resetBoard(S_BOARD *pos);
extern int parseFEN(char *fen, S_BOARD *pos);
extern void printBoard(const S_BOARD *pos);
extern void updateListsMaterial(S_BOARD *pos);
extern int checkBoard(const S_BOARD *pos);

//init.c

extern void init();

//bitboards.c

extern void printBitboard(u64 bb);
extern int popBit(u64 *bb);
extern int countBits(u64 b);

//attack.c

extern int sqAttacked(const int sq, const int side, const S_BOARD *pos);

//io.c

extern char *prmove(const int move);
extern char *prsq(const int sq);
extern void printmvlist(const S_MOVELIST *list);
extern int parsemove(const char *ptrchar, S_BOARD *pos);

//validate.c

extern int sqOnBoard(const int sq);
extern int sideValid(const int side);
extern int fileRankValid(const int fr);
extern int pieceValidEmpty(const int pce);
extern int pieceValid(const int pce);
//extern void mirrorEvalTest(S_BOARD *pos);
extern int sqIs120(const int sq);
extern int pceValidEmptyOffbrd(const int pce);
//extern int moveListOk(const S_MOVELIST *list,  const S_BOARD *pos);

//movegen.c
extern void generateAllMoves(const S_BOARD *pos, S_MOVELIST *movelist);

//makemove.c
extern int makeMove(S_BOARD *pos, int move);
extern void takeMove(S_BOARD *pos);

//perft.c
extern void perftTest(S_BOARD *pos, int depth);

//search.c
extern void searchPosition(S_BOARD *pos);

#endif //BCE_DEFS_H
