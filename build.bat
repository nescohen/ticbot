@echo off
gcc -std=c99 -g -Wall -O2 -static -o bot tictac.c
rm -f tictac.zip
7z a -tzip tictac.zip tictac.c > nul
