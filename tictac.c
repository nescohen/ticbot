/* Nes Cohen 5/9/16
   Ultimate Tic Tac Toe Bot v1 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

// #define DEBUG

#define BOARD_SPACES 81
#define BOARD_MACROS 9
#define BOARD_PITCH 9

#define WINNING_BOARDS 8

#define PLY 4

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

typedef struct item Item;
typedef struct board Board;

typedef struct node Node;
struct node
{
	Item *children;
	Node *parent;
	int depth; /* maybe */
	long score;
	char to_move;
	Board *position;
};

struct item
{
	Node *node;
	Item *next;
};

typedef struct tree Tree;
struct tree
{
	int nodes;
	Node *root;
};

typedef struct move Move;
struct move
/* each may only be 0-8 inclusive */
{
	char x;
	char y;
};

typedef struct movelist Movelist;
struct movelist
{
	Move move;
	Movelist *next;
};

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

/* string name for current bot */
char *g_this_bot_name = NULL;
/* integer representation of bot */
char g_this_bot_id = 0;
char g_opps_bot_id = 0;

int g_input_count = 0;

Board g_current_board;

void debug_print_board(Board *board)
{
	fprintf(stderr, "Move: %d, Round: %d\n", board->moves, board->rounds);
	fprintf(stderr, "Macros: [ ");
	int i;
	for (i = 0; i < BOARD_MACROS; i++)
	{
		fprintf(stderr, "%d ", board->boards[i]);
	}
	fprintf(stderr, "]\nBoard:\n");
	for (i = 0; i < BOARD_SPACES; i++)
	{
		fprintf(stderr, "%d ", board->spaces[i]);
		if (i % 9 == 8)
		{
			fprintf(stderr, "\n");
		}
	}
}

void debug_print_node(Node *node)
{
	if (node == NULL) return;
	Item *curr = node->children;
	fprintf(stderr, "Node, score=%d, depth=%d\n", node->score, node->depth);
	debug_print_board(node->position);
	while (curr != NULL)
	{
		debug_print_node(curr->node);
		curr = curr->next;
	}
}

void debug_print_tree(Tree *tree)
{
	if (tree != NULL) debug_print_node(tree->root);
}

void debug_print_list(Movelist *list)
{
	Movelist *curr = list;
	while (curr != NULL)
	{
		fprintf(stderr, "Move (%d, %d)\n", curr->move.x, curr->move.y);
		curr = curr->next;
	}
}

Movelist *get_movelist()
{
	Movelist *result = malloc(sizeof(Movelist));
	return result;
}

void free_movelist(Movelist *list)
{
	while (list != NULL)
	{
		Movelist *temp = list->next;
		free(list);
		list = temp;
	}
}

Board *get_board()
{
	Board *result = malloc(sizeof(Board));
	return result;
}

void free_board(Board *board)
{
	free(board);
}

Tree *get_tree()
{
	Tree *tree = malloc(sizeof(Tree));
	return tree;
}

Node *get_node()
{
	Node *node = malloc(sizeof(Node));
	return node;
}

void free_node(Node *node)
{
	free_board(node->position);
	Item *curr = node->children;
	while (curr != NULL)
	{
		Item *temp = curr;
		free_node(curr->node);
		curr = curr->next;
		free(temp);
	}
	free(node);
}

void free_tree(Tree *tree)
{
	free_node(tree->root);
	free(tree);
}

Item *get_item()
{
	Item *item = malloc(sizeof(Item));
	return item;
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
		else if (board->spaces[offset] == g_this_bot_id)
		{
			score += 100;
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

long minimax_node(Node *node, int max_player)
{
	if (node->children == NULL)
	{
		return node->score;
	}

	if (max_player)
	{
		long best = LONG_MIN;
		Item *curr = node->children;
		while (curr != NULL)
		{
			long score = minimax_node(curr->node, 0);
			if (score > best)
			{
				best = score;
			}
			curr = curr->next;
		}
		return best;
	}
	else
	{
		long best = LONG_MAX;
		Item *curr = node->children;
		while (curr != NULL)
		{
			long score = minimax_node(curr->node, 1);
			if (score < best)
			{
				best = score;
			}
			curr = curr->next;
		}
		return best;
	}
}

Move minimax(Tree *tree)
{
	Item *curr = tree->root->children;

	while(curr != NULL)
	{
		curr->node->score = minimax_node(curr->node, 0);
		curr = curr->next;
	}
}

void move_and_update(Board *board, Move *move, char id)
/* warning: does not verify if the move is legal */
{
	int macro_x = move->x / 3;
	int macro_y = move->y / 3;
	int macroboard = macro_x + macro_y*3;
	Move micromove;
	micromove.x = move->x - g_macro_table[macroboard].x;
	micromove.y = move->y - g_macro_table[macroboard].y;
	int next_macro = micromove.x + micromove.y*3;

	board->spaces[move->x + move->y*BOARD_PITCH] = id;
	int microboard = g_macro_table[macroboard].x + g_macro_table[macroboard].y*BOARD_PITCH;
	int result = evaluate_board(board->spaces + microboard, 3, 3, BOARD_PITCH);
	if (result > 0)
	{
		board->boards[macroboard] = result;
	}
	else if (result == -1)
	{
		/* tied board */
		board->boards[macroboard] = 3;
	}

	if (board->boards[next_macro] <= 0)
	{
		int i;
		for (i = 0; i < BOARD_MACROS; i++)
		{
			if (board->boards[i] == -1)
			{
				board->boards[i] = 0;
			}
		}
		board->boards[next_macro] = -1;
	}
	else
	{
		int i;
		for (i = 0; i < BOARD_MACROS; i++)
		{
			if (board->boards[i] == 0)
			{
				board->boards[i] = -1;
			}
		}
	}
}

Movelist *legal_moves(Board *state)
{
	Movelist *result;
	Movelist *curr;
	int i, j, k;

	result = NULL;
	curr = NULL;
	for (i = 0; i < BOARD_MACROS; i++)
	{
		if (state->boards[i] == -1)
		{
			Move translate = g_macro_table[i];
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < 3; k++)
				{
					int space = translate.x + j + (translate.y + k)*BOARD_PITCH;
					if (state->spaces[space] == 0)
					{
						Move add = {.x = translate.x + j, .y = translate.y + k};
						if (curr == NULL)
						{
							result = get_movelist();
							curr = result;
						}
						else
						{
							curr->next = get_movelist();
							curr = curr->next;
						}
						curr->move = add;
						curr->next = NULL;
					}
				}
			}
		}
	}

#ifdef DEBUG
//	if (result == NULL) fprintf(stderr, "Bot believes there are no legal moves\n");
#endif

	return result;
}

void fill_children(Node *node)
{
	Movelist *list = legal_moves(node->position);
	Item *board_list = NULL;
	Item *curr_board = NULL;
	Movelist *curr = list;

	while (curr != NULL)
	{
		if (curr_board == NULL)
		{
			board_list = get_item();
			curr_board = board_list;
		}
		else
		{
			curr_board->next = get_item();
			curr_board = curr_board->next;
		}

		curr_board->node = get_node();
		curr_board->next = NULL;
		Node *curr_node = curr_board->node;
		curr_node->parent = node;
		curr_node->children = NULL;
		curr_node->depth = node->depth + 1;
		curr_node->position = get_board();
		curr_node->to_move = abs(node->to_move - 3);
		*(curr_node->position) = *(node->position);
		memcpy(curr_node->position, node->position, sizeof(Board));
		move_and_update(curr_node->position, &curr->move, node->to_move);
		curr_node->score = score_board(curr_node->position);

		curr = curr->next;
	}

	free_movelist(list);

	node->children = board_list;
}

Tree *construct_tree(Board *current_state, char to_move)
{
	Tree *tree = get_tree();

	tree->root = get_node();
	tree->root->position = current_state;
	tree->root->to_move = to_move;
	tree->root->parent = NULL;
	tree->root->children = NULL;
	tree->root->depth = 0;

	Node *curr = tree->root;
	Item *stack = NULL;
	while (curr != NULL)
	{
		if (curr->depth < PLY)
		{
			fill_children(curr);
			Item *curr_node = curr->children;
			while(curr_node != NULL)
			{
				Item *temp = stack;
				stack = get_item();
				stack->node = curr_node->node;
				stack->next = temp;
				curr_node = curr_node->next;
			}
		}

		if (stack == NULL)
		{
			curr = NULL;
		}
		else
		{
			Item *temp = stack;
			curr = stack->node;
			stack = stack->next;
			free(temp);
		}
	}

	return tree;
}

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

Move reverse_move(Board *original, Board *changed)
{
	int i;
	for (i = 0; i < BOARD_SPACES; i++)
	{
		if (original->spaces[i] != changed->spaces[i])
		{
			Move result;
			result.x = i % BOARD_PITCH;
			result.y = i / BOARD_PITCH;
			return result;
		}
	}
	Move result;
	result.x = -1;
	result.y = -1;
	return result;
}

Move make_move(int milliseconds, Board *curr_board)
{
	fprintf(stderr, "Time Bank: %d milliseconds\n", milliseconds);

	if (g_this_bot_id == 0)
	{
		Move result = {.x = 0, .y = 0};
		fprintf(stderr, "Something went very wrong...\n");
		return result;
	}
	Board *root = get_board();
	memcpy(root, curr_board, sizeof(Board));
	Tree *tree = construct_tree(root, g_this_bot_id);
	minimax(tree);

	Move result;
	long best = LONG_MIN;
	Item *curr = tree->root->children;
	while (curr != NULL)
	{
		if (curr->node->score > best)
		{
			best = curr->node->score;
			result = reverse_move(tree->root->position, curr->node->position);
		}
		curr = curr->next;
	}

	fprintf(stderr, "Best Move Score: %d\n", best);

	free_tree(tree);

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

#ifdef DEBUG
	fprintf(stderr, "%s\n", buffer);
#endif

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
			g_time_bank = strtol(op_3, NULL, 10);
			Move made = make_move(strtol(op_3, NULL, 10), &g_current_board);
#ifdef DEBUG
			fprintf(stderr, "about to print move. Input count=%d\n", g_input_count);
#endif
			fprintf(stdout, "%s %d %d\n", STR_PLACE_MOVE, made.x, made.y);
			fflush(stdout);
			return 0;
		}
		else return 1;
	}
	else return 1;
}

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