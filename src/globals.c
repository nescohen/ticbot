#include "definitions.h"
#include "globals.h"

const Move g_macro_table[9] =
{
	{.x = 0, .y = 0},
	{.x = 3, .y = 0},
	{.x = 6, .y = 0},
	{.x = 0, .y = 3},
	{.x = 3, .y = 3},
	{.x = 6, .y = 3},
	{.x = 0, .y = 6},
	{.x = 3, .y = 6},
	{.x = 6, .y = 6},
};

const uint16_t g_winning_boards[8] =
{
	0x007, /* 0b 0 0000 0111 - vertical top row */
	0x038, /* 0b 0 0011 1000 - vertical middle row */
	0x1C0, /* 0b 1 1100 0000 - vertical bottom row */
	0x049, /* 0b 0 0100 1001 - horizontal left row */
	0x092, /* 0b 0 1001 0010 - horizontal middle row */
	0x124, /* 0b 1 0010 0100 - horizontal right row */
	0x111, /* 0b 1 0001 0001 - diagonal top right to bottom left */
	0x054, /* 0b 0 0101 0100 - diagonal top left to bottom right */
};

/* global time variables */
int g_starting_time = 0;
int g_time_per_turn = 0;
int g_time_bank = 0;
/* all in milliseconds */
clock_t g_move_start_time = 0;

/* string name for current bot */
char *g_this_bot_name = NULL;
/* integer representation of bot */
char g_this_bot_id = 0;
char g_opps_bot_id = 0;

int g_input_count = 0;

Board g_current_board;

#ifdef DEBUG_MINIMAX
clock_t g_minimax_time = 0;
#endif