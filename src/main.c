/* Nes Cohen 5/9/16
   Ultimate Tic Tac Toe Bot v4, organized bitches
   less EXTREMELY PRELIMINARY */

#include "definitions.h"

int main(int argc, char const *argv[])
{

#ifndef DEBUG
	int error = 0;
	while(!error)
	{
		error = get_input();
	}
	return error;
#endif

#ifdef DEBUG
	g_this_bot_id = 1;
	g_opps_bot_id = 2;
	memset(g_current_board.boards, -1, BOARD_MACROS);
	memset(g_current_board.spaces, 0, BOARD_SPACES);
	Tree *test = construct_tree(&g_current_board, 1);
	debug_print_tree(test);
	return 0;
#endif
}