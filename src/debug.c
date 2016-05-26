#include "debug.h"
#include "definitions.h"
#include <stdio.h>

#ifdef DEBUG

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
	fprintf(stderr, "Node, score=%ld, depth=%d\n", node->score, node->depth);
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

void debug_print_nodelist(Item *list)
{
	Item *curr = list;
	while (curr != NULL)
	{
		if (curr->node != NULL) debug_print_node(curr->node);
		else fprintf(stderr, "NULL\n");
		curr = curr->next;
	}
}

#endif