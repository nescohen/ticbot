
build:
	gcc -std=c99 -g -Wall -O2 -o bin/bot src/board.c src/debug.c src/globals.c src/in_out.c src/main.c src/memory.c src/tree.c
