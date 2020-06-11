cmake_minimum_required(VERSION 2.8.4)
project(badchessengine C)

set(CMAKE_C_STANDARD 99)

add_executable(badchessengine main.c init.c bitboards.c hashkeys.c board.c attack.c io.c movegen.c validate.c makemove.c perft.c search.c util.c tt.c evaluate.c uci.c validate.h board.h attack.h bitboards.h evaluate.h hashkeys.h init.h io.h makemove.h movegen.h perft.h search.h tt.h uci.h util.h types.h types.c psqt.c psqt.h)