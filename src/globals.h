#ifndef GLOBALS_H
#define GLOBALS_H

#include "definitions.h"

extern const Move g_macro_table[9];

extern const uint16_t g_winning_boards[8];

/* global time variables */
extern int g_starting_time;
extern int g_time_per_turn;
extern int g_time_bank;
/* all in milliseconds */
extern clock_t g_move_start_time;

/* string name for current bot */
extern char *g_this_bot_name = NULL;
/* integer representation of bot */
extern char g_this_bot_id = 0;
extern char g_opps_bot_id = 0;

extern int g_input_count = 0;

extern Board g_current_board;

#ifdef DEBUG_MINIMAX
extern clock_t g_minimax_time;
#endif

#endif