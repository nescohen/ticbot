@echo off
gcc -std=c99 -g -Wall -O2 -static -o bin\bot src\board.c src\debug.c src\globals.c src\in_out.c src\main.c src\memory.c src\tree.c
rm -f tictac.zip
pushd src
7z a -tzip ..\tictac.zip board.c debug.c globals.c in_out.c main.c memory.c tree.c > nul
popd
