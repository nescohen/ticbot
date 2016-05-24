#include "board.h"
#include "tree.h"
#include "definitions.h"
#include "globals.h"
#include "memory.h"

#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

static Movelist *legal_moves(Board *state)
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

static void move_and_update(Board *board, Move *move, char id)
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

static void fill_children(Node *node)
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
		memcpy(curr_node->position, node->position, sizeof(Board));
		curr_node->position->last_move = curr->move;
		move_and_update(curr_node->position, &curr->move, node->to_move);
		curr_node->score = score_board(curr_node->position);

		curr = curr->next;
	}

	free_movelist(list);

	node->children = board_list;
}


static long prune_node(Node *node, int depth, long alpha, long beta, int max_player)
{
	Item *curr;

	if (node->depth >= depth || node->score == LONG_MAX || node->score == LONG_MIN)
	{
		return node->score;
	}

	fill_children(node);
	curr = node->children;
	if (max_player)
	{
		long value = LONG_MIN;
		while(curr != NULL)
		{
			long child = prune_node(curr->node, depth, alpha, beta, 0);
			if (curr->node->depth > 1)
			{
				free_node(curr->node);
				curr->node = NULL;
			}
			if (child > value) value = child;
			if (value > alpha) alpha = value;
			if (beta <= alpha) break;
			curr = curr->next;
		}
		node->score = value;
		return value;
	}
	else /* min player */
	{
		long value = LONG_MAX;
		while(curr != NULL)
		{
			long child = prune_node(curr->node, depth, alpha, beta, 1);
			if (curr->node->depth > 1)
			{
				free_node(curr->node);
				curr->node = NULL;
			}
			if (child < value) value = child;
			if (value < beta) beta = value;
			if (beta <= alpha) break;
			curr = curr->next;
		}
		node->score = value;
		return value;
	}
}

Tree *construct_tree_ab(Board *current_state, char to_move, int ply, int millis)
{
	Tree *tree = get_tree();

	tree->root = get_node();
	tree->root->position = current_state;
	tree->root->to_move = to_move;
	tree->root->parent = NULL;
	tree->root->children = NULL;
	tree->root->depth = 0;
	tree->root->score = 0;

	tree->root->score = prune_node(tree->root, ply, LONG_MIN, LONG_MAX, 1);

	return tree;
}

#ifndef ALPHA_BETA

static long minimax_node(Node *node, int max_player)
//depreciated
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

static void minimax(Tree *tree)
//depreciated
{
	Item *curr = tree->root->children;

	while(curr != NULL)
	{
		curr->node->score = minimax_node(curr->node, 0);
		curr = curr->next;
	}
}

static Tree *construct_tree(Board *current_state, char to_move, int ply, int millis)
//depreciated
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
	while (curr != NULL && (clock() - g_starting_time) / (CLOCKS_PER_SEC / 1000) < millis)
	{
		if (curr->depth < ply &&
		    curr->score != LONG_MAX &&
		    curr->score != LONG_MIN)
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

#endif