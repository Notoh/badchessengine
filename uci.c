#include "uci.h"
#include "board.h"
#include "validate.h"
#include "util.h"
#include "search.h"
#include "movegen.h"
#include "makemove.h"
#include "attack.h"
#include "evaluate.h"
#include "io.h"
#include "tt.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

static void uciInfo() {
    printf("id name %s\n", NAME);
    printf("id author notoh\n");
    printf("option name Hash type spin default %d min %d max %d\n", 64, 2, 1024); //TODO make these constants
    printf("uciok\n");
}

void uciLoop(S_BOARD *pos, S_SEARCHINFO *info) {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char line[INPUTBUFFER];
    uciInfo();

    int MB = 64;

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
            uciInfo();
        } else if(!strncmp(line, "setoption name Hash value ", 26)) {
            sscanf(line,"%*s %*s %*s %*s %d",&MB);
            if(MB < 4) MB = 4;
            if(MB > 1024) MB = 1024;
            printf("Set Hash to %d MB\n", MB);
            initHashTable(pos->hashtable, MB);
        }
        if(info->quit) {
            break;
        }
    }
}

static int threefold(const S_BOARD *pos) {
    int r = 0;
    for(int i = 0; i < pos->histPly; i++) {
        if(pos->history[i].posKey == pos->posKey) {
            r++;
        }
    }
    return r;
}

static int DrawMaterial(const S_BOARD *pos) {

    if (pos->pceNum[wP] || pos->pceNum[bP]) return FALSE;
    if (pos->pceNum[wQ] || pos->pceNum[bQ] || pos->pceNum[wR] || pos->pceNum[bR]) return FALSE;
    if (pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1) {return FALSE;}
    if (pos->pceNum[wN] > 1 || pos->pceNum[bN] > 1) {return FALSE;}
    if (pos->pceNum[wN] && pos->pceNum[wB]) {return FALSE;}
    if (pos->pceNum[bN] && pos->pceNum[bB]) {return FALSE;}

    return TRUE;
}

static int checkresult(S_BOARD *pos) {
    if(pos->fiftyMove > 100) {
        printf("1/2-1/2 {fifty move rule (claimed by BCE)}\n");
        return TRUE;
    }
    if(threefold(pos) >= 2) {
        printf("1/2-1/2 {3-fold repetition (claimed by BCE)}\n");
        return TRUE;
    }
    if(DrawMaterial(pos) == TRUE) {
        printf("1/2-1/2 {insufficient material (claimed by BCE)}\n");
    }

    S_MOVELIST list[1];
    generateAllMoves(pos, list);
    int moveNum = 0;
    int found = 0;
    for(moveNum = 0; moveNum < list->count; moveNum++) {
        if(!makeMove(pos, list->moves[moveNum].move)) {
            continue;
        }
        found++;
        takeMove(pos);
        break;
    }
    if(found != 0) {
        return FALSE;
    }

    int inCheck = sqAttacked(pos->kingSq[pos->side], pos->side^1, pos);
    if(inCheck == TRUE) {
        if(pos->side == WHITE) {
            printf("0-1 {black mates (claimed by BCE)}\n");
        } else {
            printf("1-0 {white mates (claimed by BCE)}\n");
        }
    } else {
        printf("1/2-1/2 {stalemate (claimed by BCE)}\n");
    }
    return TRUE;
}


//only used if CONSOLE is defined
void consoleLoop(S_BOARD *pos, S_SEARCHINFO *info) {

    printf("Welcome to BCE In Console Mode!\n");
    printf("Type help for commands\n\n");

    info->POST_THINKING = TRUE;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int depth = MAXDEPTH, movetime = 3000;
    int engineSide = BOTH;
    int move = NOMOVE;
    char inBuf[80], command[80];

    engineSide = BLACK;
    parseFEN(START_FEN, pos);

    while(TRUE) {

        fflush(stdout);

        if(pos->side == engineSide && checkresult(pos) == FALSE) {
            info->start = getTimeMs();
            info->depth = depth;

            if(movetime != 0) {
                info->timeset = TRUE;
                info->stop = info->start + movetime;
            }

            searchPosition(pos, info);
        }

        printf("\nBCE > ");

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if (!fgets(inBuf, 80, stdin))
            continue;

        sscanf(inBuf, "%s", command);

        if(!strcmp(command, "help")) {
            printf("Commands:\n");
            printf("quit - quit game\n");
            printf("force - computer will not think\n");
            printf("print - show board\n");
            printf("post - show thinking\n");
            printf("nopost - do not show thinking\n");
            printf("new - start new game\n");
            printf("go - set computer thinking\n");
            printf("depth x - set depth to x\n");
            printf("time x - set thinking time to x seconds (depth still applies if set)\n");
            printf("view - show current depth and movetime settings\n");
            printf("** note ** - to reset time and depth, set to 0\n");
            printf("enter moves using b7b8q notation\n\n\n");
            continue;
        }

        if(!strcmp(command, "mirror")) {
            engineSide = BOTH;
            mirrorEvalTest(pos);
            continue;
        }

        if(!strcmp(command, "eval")) {
            printBoard(pos);
            printf("Eval:%d",eval(pos));
            mirrorBoard(pos);
            printBoard(pos);
            printf("Eval:%d",eval(pos));
            continue;
        }


        if(!strcmp(command, "quit")) {
            info->quit = TRUE;
            break;
        }

        if(!strcmp(command, "post")) {
            info->POST_THINKING = TRUE;
            continue;
        }

        if(!strcmp(command, "print")) {
            printBoard(pos);
            continue;
        }

        if(!strcmp(command, "nopost")) {
            info->POST_THINKING = FALSE;
            continue;
        }

        if(!strcmp(command, "force")) {
            engineSide = BOTH;
            continue;
        }

        if(!strcmp(command, "view")) {
            if(depth == MAXDEPTH) printf("depth not set ");
            else printf("depth %d",depth);

            if(movetime != 0) printf(" movetime %ds\n",movetime/1000);
            else printf(" movetime not set\n");

            continue;
        }

        if(!strcmp(command, "depth")) {
            sscanf(inBuf, "depth %d", &depth);
            if(depth==0) depth = MAXDEPTH;
            continue;
        }

        if(!strcmp(command, "time")) {
            sscanf(inBuf, "time %d", &movetime);
            movetime *= 1000;
            continue;
        }

        if(!strcmp(command, "new")) {
            engineSide = BLACK;
            parseFEN(START_FEN, pos);
            continue;
        }

        if(!strcmp(command, "go")) {
            engineSide = pos->side;
            continue;
        }

        move = parsemove(inBuf, pos);
        if(move == NOMOVE) {
            printf("Command unknown:%s\n",inBuf);
            continue;
        }
        makeMove(pos, move);
        pos->ply=0;
    }
}

//from Weiss
int mateScore(const int score) {
    return score > 0 ? ((INFINITE - score) / 2) + 1
                        : -((INFINITE + score) / 2);
}

//largely based on weiss
void printThinking(const S_BOARD *pos, int depth, int score, long nodes, long elapsed, int pvMoves) {

    //mate or centipawn score
    char *type = abs(score) >= ISMATE ? "mate" : "cp";


    score = abs(score) >= ISMATE ? mateScore(score)
            : score;

    int hash = hashFull(pos);
    int nps = (int)(1000 * nodes / (elapsed + 1));

    printf("info depth %d score %s %d time %ld nodes %ld nps %d hashfull %d pv", depth, type, score, elapsed, nodes, nps, hash);

    for(int i = 0; i < pvMoves; i++) {
        printf(" %s", prmove(pos->pvarray[i]));
    }

    printf("\n");
    fflush(stdout);
}

void printConclusion(const S_BOARD *pos, int move) {
    printf("bestmove %s", prmove(move));
    printf("\n");
    fflush(stdout);
}