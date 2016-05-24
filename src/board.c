#include "board.h"
#include "definitions.h"
#include "globals.h"
#include <stdint.h>
#include <limits.h>

static long value_microboard(const char *board, int pitch)
{
	long score = 0;
	int i, j;
	for (i = 0; i < 3; i++)
	{
		int horizontal_count = 0;
		int vertical_count = 0;
		for(j = 0; j < 3; j++)
		{
			if (*(board + i + j*pitch) == g_this_bot_id)
			{
				horizontal_count += 1;
			}
			else if (*(board + i + j*pitch) == g_opps_bot_id)
			{
				horizontal_count -= 1;
			}
			if (*(board + j + i*pitch) == g_this_bot_id)
			{
				vertical_count += 1;
			}
			else if (*(board + j + i*pitch) == g_opps_bot_id)
			{
				vertical_count -= 1;
			}
		}
		if (horizontal_count >= 2)
		{
			score += 150;
		}
		else if (horizontal_count <= -2)
		{
			score -= 150;
		}
		if (vertical_count >= 2)
		{
			score += 150;
		}
		else if (vertical_count <= -2)
		{
			score -= 150;
		}
	}

	int fore_count = 0;
	int back_count = 0;
	for (i = 0; i < 3; i++)
	{
		if (*(board + i + i*pitch) == g_this_bot_id)
		{
			fore_count += 1;
		}
		if (*(board + i + (2 - i)*pitch) == g_this_bot_id)
		{
			fore_count += 1;
		}
	}
	if (fore_count >= 2)
	{
		score += 150;
	}
	else if (fore_count <= -2)
	{
		score -= 150;
	}
	if (back_count >= 2)
	{
		score += 150;
	}
	else if (back_count <= -2)
	{
		score -= 150;
	}

	/* check the corners */
	if (*board == g_this_bot_id) score += 10;
	if (*(board + 2) == g_this_bot_id) score += 10;
	if (*(board + 2*pitch) == g_this_bot_id) score += 10;
	if (*(board + 2 + 2*pitch) == g_this_bot_id) score += 10;

	return score;
}

long score_board(Board *board)
{
	/* determine if a board is in an endgame position */
	uint16_t this_bot = 0;
	uint16_t opposing_bot = 0;
	char won = 0;
	int i;

	for (i = 0; i < BOARD_MACROS; i++)
	{
		if (board->boards[i] == g_this_bot_id)
		{
			this_bot |= 1 << i;
		}
		else if (board->boards[i] == g_opps_bot_id)
		{
			opposing_bot |= 1 << i;
		}
	}

	for (i = 0; i < WINNING_BOARDS; i++)
	{
		if ((g_winning_boards[i] & this_bot) == g_winning_boards[i])
		{
			won = g_this_bot_id;
		}
		else if ((g_winning_boards[i] & opposing_bot) == g_winning_boards[i])
		{
			won = g_opps_bot_id;
		}
	}

	if (won == g_this_bot_id)
	{
		return LONG_MAX;
	}
	else if (won == g_opps_bot_id)
	{
		return LONG_MIN;
	}

	long score = 0;
	if (board->spaces[40] == g_this_bot_id) /* test for the very middle square */
	{
		score += 100;
	}
	if (board->boards[4] == g_this_bot_id) /* test for the very middle board */
	{
		score += 1000;
	}
	for (i = 0; i < BOARD_MACROS; i++)
	{
		int offset = g_macro_table[i].x + g_macro_table[i].y*BOARD_PITCH;
		if (board->boards[i] == g_this_bot_id)
		{
			score += 1000;
		}
		else if (board->boards[i] == g_opps_bot_id)
		{
			score -= 1000;
		}
		else if (board->spaces[offset + 10] == g_this_bot_id)
		{
			score += 50;
		}
		else if (board->spaces[offset + 10] == g_opps_bot_id)
		{
			score -= 50;
		}
		if (board->boards[i] <= 0)
		{
			score += value_microboard(board->spaces + offset, BOARD_PITCH);
		}
	}

	return score;
}

int evaluate_board(const char *board, int rows, int columns, int pitch)
{
	uint16_t bot_1 = 0;
	uint16_t bot_2 = 0;
	int bot_1_won = 0;
	int bot_2_won = 0;
	int i, j;

	for (i = 0; i < columns; i++)
	{
		for (j = 0; j < rows; j++)
		{
			if (*(board + i + j*pitch) == 1)
			{
				bot_1 |= 1 << (i + j*3);
			}
			else if (*(board + i + j*pitch) == 2)
			{
				bot_2 |= 1 << (i + j*3);
			}
		}
	}

	for (i = 0; i < WINNING_BOARDS; i++)
	{
		if ((bot_1 & g_winning_boards[i]) == g_winning_boards[i])
		{
			bot_1_won = 1;
		}
		if ((bot_2 & g_winning_boards[i]) == g_winning_boards[i])
		{
			bot_2_won = 1;
		}
	}

#ifdef DEBUG
	if (bot_1_won && bot_2_won)
	{
		fprintf(stderr, "Bot reports both players won on same board\n");
	}
#endif

	if (bot_1_won)
	{
		return 1;
	}
	else if (bot_2_won)
	{
		return 2;
	}
	else if ((bot_1 & bot_2) == 0x1FF)
	{
		return -1; /* tied board */
	}
	else
	{
		return 0;
	}
}