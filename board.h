#ifndef BADCHESSENGINE_BOARD_H
#define BADCHESSENGINE_BOARD_H

extern void resetBoard(S_BOARD *pos);
extern int parseFEN(char *fen, S_BOARD *pos);
extern void printBoard(const S_BOARD *pos);
extern void updateListsMaterial(S_BOARD *pos);
extern int checkBoard(const S_BOARD *pos);
extern void mirrorBoard(S_BOARD *pos);

#endif //BADCHESSENGINE_BOARD_H
