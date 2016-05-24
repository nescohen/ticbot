#ifndef BOARD_H
#define BOARD_H

#include "definitions.h"

long score_board(Board *board);
int evaluate_board(const char *board, int rows, int columns, int pitch);

#endif