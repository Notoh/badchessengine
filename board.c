#include "defs.h"

void resetBoard(S_BOARD *pos) {

    int index = 0;
    for(index = 0; index < BRD_SQ_NUM; index++) {
        pos->pieces[index] = OFFBOARD;
    }

    for(index = 0; index < 64; index++) {
        pos->pieces[SQ120(index)] = EMPTY;
    }

    for(index = 0; index < 2; index++) {
        pos->bigPce[index] = 0;
        pos->majPce[index] = 0;
        pos->minPce[index] = 0;
        pos->material[index] = 0;
    }

    for(index = 0; index < 3; index++) {
        pos->pawns[index] = 0ULL;
    }

    for(index = 0; index < 13; index++) {
        pos->pceNum[index] = 0;
    }

    pos->kingSq[WHITE] = pos->kingSq[BLACK] = NO_SQ;
    pos->side = BOTH;
    pos->enPas = NO_SQ;
    pos->fiftyMove = 0;

    pos->ply = 0;
    pos->histPly = 0;

    pos->castlePerm = 0;

    pos->posKey = 0ULL;

}

int parseFEN(char *fen, S_BOARD *pos) {
    ASSERT(fen != NULL);
    ASSERT(pos != NULL);

    int rank = RANK_8;
    int file = FILE_A;
    int piece = 0;
    int count = 0;
    int i = 0;
    int sq64 = 0;
    int sq120 = 0;

    resetBoard(pos);

    while((rank >= RANK_1) && *fen) {
        count = 1;
        switch(*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'q': piece = bQ; break;
            case 'k': piece = bK; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'Q': piece = wQ; break;
            case 'K': piece = wK; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;
            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;
            default:
                printf("FEN error \n");
                return -1;
        }

        for(i = 0; i < count; i++) {
            sq64 = rank * 8 + file;
            sq120 = SQ120(sq64);
            if(piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
            file++;
        }
        fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b');

    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2;

    for(i = 0; i < 4; i++) {
        if(*fen == ' ') {
            break;
        }

        switch(*fen) {
            case 'K': pos->castlePerm |= WSCA; break;
            case 'Q': pos->castlePerm |= WLCA; break;
            case 'k': pos->castlePerm |= BSCA; break;
            case 'q': pos->castlePerm |= BLCA; break;
            default: break;
        }
        fen++;
    }
    fen++;

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    if(*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';

        ASSERT(file >=FILE_A && file <= FILE_H);
        ASSERT(rank >= RANK_1 && rank <= RANK_8);

        pos->enPas = FR2SQ(file,rank);
    }

    pos->posKey = generatePosKey(pos);

    updateListsMaterial(pos);

    return 0;
}

void printBoard(const S_BOARD *pos) {
    int sq, file, rank, piece;
    printf("\nGame Board: \n\n");
    for(rank = RANK_8; rank >= RANK_1; rank--) {
        printf("%d   ", rank + 1);
        for(file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file, rank);
            piece = pos->pieces[sq];
            printf("%3c", pceChar[piece]);
        }
        printf("\n");
    }
    printf("\n   ");
    for(file = FILE_A; file <= FILE_H; file++) {
        printf("%3c", 'a'+file);
    }
    printf("\n");
    printf("side:%c\n", sideChar[pos->side]);
    printf("enPas:%d\n", pos->enPas);
    printf("castle:%c%c%c%c\n",
            pos->castlePerm&WSCA?'K':'-',
            pos->castlePerm&WLCA?'Q':'-',
            pos->castlePerm&BSCA?'k':'-',
            pos->castlePerm&BLCA?'q':'-');

    printf("PosKey:%llX\n", pos->posKey);

}

void updateListsMaterial(S_BOARD *pos) {
    int piece, sq, index, colour;

    for(index = 0; index < BRD_SQ_NUM; index++) {
        sq = index;
        piece = pos->pieces[index];
        if(piece != OFFBOARD && piece != EMPTY) {
            colour = pieceCol[piece];
            if(pieceBig[piece] == TRUE) pos->bigPce[colour]++;
            if(pieceMaj[piece] == TRUE) pos->majPce[colour]++;
            if(pieceMin[piece] == TRUE) pos->minPce[colour]++;

            pos->material[colour] += pieceVal[piece];

            //piece list
            pos->pList[piece][pos->pceNum[piece]] = sq;
            pos->pceNum[piece]++;

            if(piece == wK) pos->kingSq[WHITE] = sq;
            if(piece == bK) pos->kingSq[BLACK] = sq;

            if(piece == wP) {
                SETBIT(pos->pawns[WHITE], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            } else if(piece == bP) {
                SETBIT(pos->pawns[BLACK], SQ64(sq));
                SETBIT(pos->pawns[BOTH], SQ64(sq));
            }
        }
    }
}

int checkBoard(const S_BOARD *pos) {
    int t_pceNum[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int t_bigPce[2] = {0, 0};
    int t_majPce[2] = {0, 0};
    int t_minPce[2] = {0, 0};
    int t_material[2] = {0, 0};

    int sq64, t_piece, t_pce_num, sq120, colour, pcount;

    u64 t_pawns[3] = {0ULL, 0ULL, 0ULL};

    t_pawns[WHITE] = pos->pawns[WHITE];
    t_pawns[BLACK] = pos->pawns[BLACK];
    t_pawns[BOTH] = pos->pawns[BOTH];

    //check piece lists
    for(t_piece = wP; t_piece <= bK; t_piece++) {
        for(t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; t_pce_num++) {
            sq120 = pos->pList[t_piece][t_pce_num];
            ASSERT(pos->pieces[sq120]==t_piece);
        }
    }

    //check piece count and other counters
    for(sq64 = 0;  sq64 < 64; sq64++) {
        sq120 = SQ120(sq64);
        t_piece = pos->pieces[sq120];
        t_pceNum[t_piece]++;
        colour = pieceCol[t_piece];
        if(pieceBig[t_piece]) t_bigPce[colour]++;
        if(pieceMaj[t_piece]) t_majPce[colour]++;
        if(pieceMin[t_piece]) t_minPce[colour]++;

        t_material[colour] += pieceVal[t_piece];
    }

    for(t_piece = wP; t_piece <= bK; t_piece++) {
        ASSERT(t_pceNum[t_piece]==pos->pceNum[t_piece]);
    }

    //check bitboards count
    pcount = CNT(t_pawns[WHITE]);
    ASSERT(pcount == pos->pceNum[wP]);
    pcount = CNT(t_pawns[BLACK]);
    ASSERT(pcount == pos->pceNum[bP]);
    pcount = CNT(t_pawns[BOTH]);
    ASSERT(pcount == (pos->pceNum[wP] + pos->pceNum[bP]));

    //check bitboards squares

    while(t_pawns[WHITE]) {
        sq64 = POP(&t_pawns[WHITE]);
        ASSERT(pos->pieces[SQ120(sq64)] == wP);
    }
    while(t_pawns[BLACK]) {
        sq64 = POP(&t_pawns[BLACK]);
        ASSERT(pos->pieces[SQ120(sq64)] == bP);
    }
    while(t_pawns[BOTH]) {
        sq64 = POP(&t_pawns[BOTH]);
        ASSERT(pos->pieces[SQ120(sq64)] == bP || pos->pieces[SQ120(sq64)] == wP);
    }

    ASSERT(t_material[WHITE] ==pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK]);
    ASSERT(t_minPce[WHITE] ==pos->minPce[WHITE] && t_minPce[BLACK] == pos->minPce[BLACK]);
    ASSERT(t_majPce[WHITE] ==pos->majPce[WHITE] && t_majPce[BLACK] == pos->majPce[BLACK]);
    ASSERT(t_bigPce[WHITE] ==pos->bigPce[WHITE] && t_bigPce[BLACK] == pos->bigPce[BLACK]);

    ASSERT(pos->side==WHITE || pos->side==BLACK);
    ASSERT(generatePosKey(pos)==pos->posKey);

    ASSERT(pos->enPas == NO_SQ || (ranksBrd[pos->enPas]==RANK_6 && pos->side==WHITE) || (ranksBrd[pos->enPas] == RANK_3 && pos->side==BLACK));

    ASSERT(pos->pieces[pos->kingSq[WHITE]] == wK);
    ASSERT(pos->pieces[pos->kingSq[BLACK]] == bK);

    return TRUE;

}

void mirrorBoard(S_BOARD *pos) {
    int tempPieces[64];
    int tempSide = pos->side^1;
    int swapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
    int tempCastle = 0;
    int tempEnPas = NO_SQ;

    int sq;
    int tp;

    if(pos->castlePerm & WSCA) tempCastle |= BSCA;
    if(pos->castlePerm & WLCA) tempCastle |= BLCA;

    if(pos->castlePerm & BSCA) tempCastle |= WSCA;
    if(pos->castlePerm & BLCA) tempCastle |= WLCA;

    if(pos->enPas != NO_SQ) {
        tempEnPas = SQ120(Mirror64[SQ64(pos->enPas)]);
    }

    for(sq = 0; sq < 64; sq++) {
        tempPieces[sq] = pos->pieces[SQ120(Mirror64[sq])];
    }

    resetBoard(pos);

    for(sq = 0; sq < 64; sq++) {
        tp = swapPiece[tempPieces[sq]];
        pos->pieces[SQ120(sq)] = tp;
    }

    pos->side = tempSide;
    pos->castlePerm = tempCastle;
    pos->enPas = tempEnPas;

    pos->posKey = generatePosKey(pos);

    updateListsMaterial(pos);

    ASSERT(checkBoard(pos));
}