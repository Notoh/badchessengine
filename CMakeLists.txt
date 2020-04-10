cmake_minimum_required(VERSION 2.8.4)
project(badchessengine C)

set(CMAKE_C_STANDARD 99)

add_executable(badchessengine main.c defs.h init.c bitboards.c hashkeys.c board.c data.c attack.c io.c movegen.c validate.c makemove.c perft.c search.c util.c hashtable.c evaluate.c uci.c polyglot/polybook.c polyglot/polybook.h)