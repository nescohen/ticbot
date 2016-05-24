@echo off
gcc -std=c99 -g -Wall -O2 -static -o bin\bot src\tictac.c
rm -f tictac.zip
7z a -tzip tictac.zip src\tictac.c > nul
