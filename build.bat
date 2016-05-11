@echo off
gcc -g -std=c99 -O2 -static -o bot tictac.c
rm -f tictac.zip
7z a -tzip tictac.zip tictac.c > nul
