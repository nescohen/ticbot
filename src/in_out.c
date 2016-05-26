#include "definitions.h"
#include "in_out.h"
#include "globals.h"
#include "tree.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>

static void add_macroboards(const char *string, Board *board)
{
	char *copy = (char *)malloc(strlen(string) + 1);
	char *curr;
	int i;

	strcpy(copy, string);
	curr = strtok(copy, ",");
	i = 0;
	while (curr != NULL && i < BOARD_MACROS)
	{
		board->boards[i++] = strtol(curr, NULL, 10);
		curr = strtok(NULL, ",");
	}

	free(copy);
}

static void translate_board(const char *string, Board *board)
{
	char *copy = (char *)malloc(strlen(string) + 1);
	char *curr;
	int i;

	strcpy(copy, string);
	curr = strtok(copy, ",");
	i = 0;
	while (curr != NULL && i < BOARD_SPACES)
	{
		board->spaces[i++] = strtol(curr, NULL, 10);
		curr = strtok(NULL, ",");
	}

	free(copy);
}

static int recommend_depth(Board *board, int milliseconds)
{
	int i;
	int open = 0;
	for (i = 0; i < BOARD_MACROS; i++)
	{
		if (board->boards[i] == -1)
		{
			open += 1;
		}
	}

	int result;
	if (open == 1)
	{
		if (milliseconds > 6000) result = 8;
		else result = 7;
	}
	else if (open == 2)
	{
		result = 9;
	}
	else if (open <= 5 && milliseconds > 6000)
	{
		result = 8;
	}
	else
	{
		result = 7;
	}
	return result;
}

static int settings(const char *setting, const char *value)
/* setting is a null-terminated string that represents the setting to be altered */
/* value is a pointer to the string passed into the program */
{
	switch (*setting)
	{
		case 'p':
		{
			if (strcmp(setting, STR_PLAYER_NAMES) == 0)
			{
				/* maybe do something later, maybe who gives a shit */
				return 0;
			}
			else return 1;
		} break;

		case 't':
		{
			if (strcmp(setting, STR_TIMEBANK) == 0)
			{
				g_starting_time = strtol(value, NULL, 10);
				return 0;
			}
			if (strcmp(setting, STR_TIME_MOV) == 0)
			{
				g_time_per_turn = strtol(value, NULL, 10);
				return 0;
			}
			else return 1;
		} break;

		case 'y':
		{
			if (strcmp(setting, STR_THIS_BOT_NAME) == 0)
			{
				if (g_this_bot_name != NULL) free(g_this_bot_name);
				g_this_bot_name = (char *)malloc(strlen(value) + 1);
				strcpy(g_this_bot_name, value);
				return 0;
			}
			if (strcmp(setting, STR_THIS_BOT_ID) == 0)
			{
				g_this_bot_id = strtol(value, NULL, 10);
				g_opps_bot_id = abs(g_this_bot_id - 3); /* 2 if this_bot is 1, and vice versa */
				return 0;
			}
			else return 1;
		} break;

		default:
		{
			return 1;
		} break;
	}
}

static int update_game(const char *aspect, const char *value)
/* aspect is a null-terminated string that represents the game state to be altered */
/* value is a pointer to the string passed into the program */
{
	switch (*aspect)
	{
		case 'r':
		{
			if (strcmp(aspect, STR_ROUND) == 0)
			{
				g_current_board.rounds = strtol(value, NULL, 10);
				return 0;
			}
			else return 1;
		} break;

		case 'm':
		{
			if (strcmp(aspect, STR_MOVE) == 0)
			{
				g_current_board.moves = strtol(value, NULL, 10);
				return 0;
			}
			else if (strcmp(aspect, STR_MACROBOARD) == 0)
			{
				add_macroboards(value, &g_current_board);
				return 0;
			}
			else return 1;
		} break;

		case 'f':
		{
			if (strcmp(aspect, STR_FIELD) == 0)
			{
				translate_board(value, &g_current_board);
				return 0;
			}
			else return 1;
		} break;

		default:
		{
			return 1;
		} break;
	}
}

static Move make_move(int milliseconds, Board *curr_board, Tree **tree)
{
#ifdef DEBUG
	fprintf(stderr, "Time Bank: %d milliseconds\n", milliseconds);
#endif

	if (g_this_bot_id == 0)
	{
		Move result = {.x = 0, .y = 0};
#ifdef DEBUG
		fprintf(stderr, "Something went very wrong...\n");
#endif
		return result;
	}

	if (g_current_board.moves == 1)
	{
		Move result = {.x = 4, .y = 4};
		return result;
	}

	int ply = recommend_depth(curr_board, milliseconds);

	Board *root = get_board();
	memcpy(root, curr_board, sizeof(Board));
#ifdef ALPHA_BETA
	*tree = construct_tree_ab(root, g_this_bot_id, ply, milliseconds - 300);
#endif

#ifndef ALPHA_BETA
	*tree = construct_tree(root, g_this_bot_id, ply, milliseconds -300);
#ifdef DEBUG_MINIMAX
	g_minimax_time = clock();
#endif
	minimax(*tree);
#ifdef DEBUG_MINIMAX
	clock_t elapsed = (clock() - g_minimax_time) / (CLOCKS_PER_SEC / 1000);
	fprintf(stderr, "Minimax milliseconds elapsed: %ld\n", elapsed);
#endif
#endif

	Move result = {.x = -1, .y = -1};
	long best = LONG_MIN;
	Item *curr = (*tree)->root->children;
	while (curr != NULL)
	{
		if (curr->node != NULL && curr->node->score > best)
		{
			best = curr->node->score;
			result = curr->node->position->last_move;
		}
		curr = curr->next;
	}

	if (best == LONG_MIN) result = (*tree)->root->children->node->position->last_move;

#ifdef DEBUG
	fprintf(stderr, "Best Move Score: %ld - (%d, %d)\n", best, result.x, result.y);
	int time_elapsed = (clock() - g_starting_time) / (CLOCKS_PER_SEC / 1000);
	fprintf(stderr, "Time Elapsed: %d. Depth Chosen: %d\n", time_elapsed, ply);
#endif
	return result;
}

int get_input()
/* reads one line of input and handles it appropriately */
{
	char buffer[500];
	char op_1[50];
	char op_2[50];
	char op_3[50];
	char op_4[200];

	g_input_count += 1;

	fgets(buffer, 500, stdin);

	sscanf(buffer, "%50s%50s%50s%200s", op_1, op_2, op_3, op_4);

	if (strcmp(op_1, STR_SETTINGS) == 0)
	{
		return settings(op_2, op_3);
	}
	else if (strcmp(op_1, STR_UPDATE) == 0)
	{
		if (strcmp(op_2, STR_GAME) == 0)
		{
			return update_game(op_3, op_4);
		}
		else return 1;
	}
	else if (strcmp(op_1, STR_REQUEST) == 0)
	{
		if (strcmp(op_2, STR_MOVE) == 0)
		{
			Tree *tree = NULL;
			g_time_bank = strtol(op_3, NULL, 10);
			g_starting_time = clock();
			Move made = make_move(strtol(op_3, NULL, 10), &g_current_board, &tree);
			fprintf(stdout, "%s %d %d\n", STR_PLACE_MOVE, made.x, made.y);
			fflush(stdout);
//			clock_t mem_time = clock();
#ifndef ALPHA_BETA
			if (tree != NULL) free_tree(tree);
#endif
//			fprintf(stderr, "Free took %ld milliseconds\n", (clock() - mem_time) / (CLOCKS_PER_SEC / 1000));
			return 0;
		}
		else return 1;
	}
	else return 1;
}