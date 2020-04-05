#include "defs.h"
#include <string.h>

#define INPUTBUFFER 400 * 6


//go depth 6 wtime 18000 btime 10000 binc 1000 winc 1000 movetime 1000 movestogo 40
void parseGo(char *line, S_SEARCHINFO *info, S_BOARD *pos) {

    int depth = -1, movestogo = 30, movetime = -1;
    int time = -1, inc = 0;
    char *ptr = NULL;
    info->timeset = FALSE;

    if ((ptr = strstr(line,"infinite"))) {
        ;
    }

    if ((ptr = strstr(line,"binc")) && pos->side == BLACK) {
        inc = atoi(ptr + 5);
    }

    if ((ptr = strstr(line,"winc")) && pos->side == WHITE) {
        inc = atoi(ptr + 5);
    }

    if ((ptr = strstr(line,"wtime")) && pos->side == WHITE) {
        time = atoi(ptr + 6);
    }

    if ((ptr = strstr(line,"btime")) && pos->side == BLACK) {
        time = atoi(ptr + 6);
    }

    if ((ptr = strstr(line,"movestogo"))) {
        movestogo = atoi(ptr + 10);
    }

    if ((ptr = strstr(line,"movetime"))) {
        movetime = atoi(ptr + 9);
    }

    if ((ptr = strstr(line,"depth"))) {
        depth = atoi(ptr + 6);
    }

    if(movetime != -1) {
        time = movetime;
        movestogo = 1;
    }

    info->start = getTimeMs();
    info->depth = depth;

    if(time != -1) {
        info->timeset = TRUE;
        time /= movestogo;
        time -= 50;
        info->stop = info->start + time + inc;
    }

    if(depth == -1) {
        info->depth = MAXDEPTH;
    }

    printf("time:%d start:%ld stop:%ld depth:%d timeset:%d\n",
           time,info->start,info->stop,info->depth,info->timeset);
    searchPosition(pos, info);
}

//position fen
//position startpos
// ... moves e2e4 e7e5 b7b8q
void parsePosition(char *line, S_BOARD *pos) {
    line += 9;
    char *ptrchar = line;
    if(!strncmp(line, "startpos", 8)) {
        parseFEN(START_FEN, pos);
    } else {
        ptrchar = strstr(line, "fen");
        if(ptrchar == NULL) {
            parseFEN(START_FEN, pos);
        } else {
            ptrchar += 4;
            parseFEN(ptrchar, pos);
        }
    }

    ptrchar = strstr(line, "moves");
    int move;
    if(ptrchar != NULL) {
        ptrchar += 6;
        while(*ptrchar) {
            move = parsemove(ptrchar, pos);
            if(move == NOMOVE) break;
            makeMove(pos, move);
            pos->ply = 0;
            while(*ptrchar && *ptrchar != ' ')  {
                ptrchar++;
            }
            ptrchar++;
        }
    }
    printBoard(pos);
}

void uciLoop() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char line[INPUTBUFFER];
    printf("id name %s\n", NAME);
    printf("id author notoh\n");
    printf("uciok\n");

    S_BOARD pos[1];
    S_SEARCHINFO info[1];
    pos->pvtable->pTable = NULL;
    initPvTable(pos->pvtable);

    while(TRUE) {
        memset(&line[0], 0, sizeof(line));
        fflush(stdout);
        if(!fgets(line, INPUTBUFFER, stdin)) {
            continue;
        }
        if(line[0]== '\n') {
            continue;
        }

        if(!strncmp(line, "isready", 7)) {
            printf("readyok\n");
            continue;
        } else if(!strncmp(line, "position", 8)) {
            parsePosition(line, pos);
        } else if(!strncmp(line, "ucinewgame", 10)) {
            parsePosition("position startpos\n", pos);
        } else if(!strncmp(line, "go", 2)) {
            parseGo(line, info, pos);
        } else if(!strncmp(line, "quit", 4)) {
            info->quit = TRUE;
            break;
        } else if(!strncmp(line, "uci", 3)) {
            printf("id name %s\n", NAME);
            printf("id author notoh\n");
            printf("uciok\n");
        }
        if(info->quit) {
            break;
        }
    }
    free(pos->pvtable->pTable);
}