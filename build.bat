@echo off
gcc -g -o bot tictac.c
rm -f tictac.zip
7z a -tzip tictac.zip tictac.c > nul
