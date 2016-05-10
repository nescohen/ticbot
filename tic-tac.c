/* Nes Cohen 5/9/16
   Ultimate Tic Tac Toe Bot v0 */

/* PLEASE NOTE: this bot is currently in version 0 which is merely
 * a technical test run. It should be fully functional, but will
 * make arbitrary legal moves. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #define DEBUG */

#define BOARD_SPACES 81
#define BOARD_MACROS 9
#define BOARD_PITCH 9

#define STR_SETTINGS "settings"
#define STR_UPDATE "update"
#define STR_REQUEST "action"

#define STR_MOVE "move"
#define STR_GAME "game"

#define STR_TIMEBANK "timebank"
#define STR_TIME_MOV "time_per_move"
#define STR_PLAYER_NAMES "player_names"
#define STR_THIS_BOT_NAME "your_bot"
#define STR_THIS_BOT_ID "your_botid"

#define STR_ROUND "round"
#define STR_FIELD "field"
#define STR_MACROBOARD "macroboard"

#define STR_PLACE_MOVE "place_move"

typedef struct move Move;
struct move
/* each may only be 0-8 inclusive */
{
	char x;
	char y;
};

typedef struct board Board;
struct board
{
	int moves;
	int rounds;
	char boards[BOARD_MACROS];
	char spaces[BOARD_SPACES];
};

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

/* global time variables */
static int g_starting_time = 0;
static int g_time_bank = 0;
static int g_time_per_turn = 0;
/* all in milliseconds */

/* string name for current bot */
static char *g_this_bot_name = NULL;
/* integer representation of bot */
static int g_this_bot_id = 0;

static Board g_current_board;

void translate_board(const char *string, Board *board)
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

void add_macroboards(const char *string, Board *board)
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

int settings(const char *setting, const char *value)
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

int update_game(const char *aspect, const char *value)
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

Move make_move(int time, Board *curr_board)
/* NOTE - temporary, make arbitrary legal move */
{
	Move result;
	int i;
	int macroboard = -1;
	for (i = 0; i < BOARD_MACROS; i++)
	{
		if (curr_board->boards[i] == -1)
		{
			macroboard = i;
		}
	}

	if (macroboard == -1)
	{
		/* no legal moves? */
		fprintf(stderr, "bot thinks there are currently no legal moves,\n");
		result.x = 0;
		result.y = 0;
		return result;
	}

	Move translation = g_macro_table[macroboard];
	
	int j;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			int square = curr_board->spaces[i + translation.x + (j + translation.y)*BOARD_PITCH];
			if (square == 0)
			{
				result.x = translation.x + i;
				result.y = translation.y + j;
			}
		}
	}
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

	fgets(buffer, 500, stdin);
	sscanf(buffer, "%50s%50s%50s%200s", op_1, op_2, op_3, op_4);

#ifdef DEBUG
	printf("1 - %s\n2 - %s\n3 - %s\n4 - %s\n", op_1, op_2, op_3, op_4);
	return 0;
#endif

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
			Move made = make_move(strtol(op_3, NULL, 10), &g_current_board);
			printf("%s %d %d\n", STR_PLACE_MOVE, made.x, made.y);
			return 0;
		}
		else return 1;
	}
	else return 1;
}

int main()
{
	int error = 0;
	while(!error)
	{
		error = get_input();
	}
	return error;
}