#include "memory.h"
#include "definitions.h"

#include <stdio.h>
#include <stdlib.h>

Movelist *get_movelist()
{
	Movelist *result = malloc(sizeof(Movelist));
	if (result == NULL) fprintf(stderr, "Uh-oh, out of memory\n");
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
	if (result == NULL) fprintf(stderr, "Uh-oh, out of memory\n");
	return result;
}

void free_board(Board *board)
{
	free(board);
}

Tree *get_tree()
{
	Tree *tree = malloc(sizeof(Tree));
	if (tree == NULL) fprintf(stderr, "Uh-oh, out of memory\n");
	return tree;
}

Node *get_node()
{
	Node *node = malloc(sizeof(Node));
	if (node == NULL) fprintf(stderr, "Uh-oh, out of memory\n");
	return node;
}

void free_node(Node *node)
{
	free_board(node->position);
	Item *curr = node->children;
	while (curr != NULL)
	{
		Item *temp = curr;
		if (curr->node != NULL) free_node(curr->node);
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
	if (item == NULL) fprintf(stderr, "Uh-oh, out of memory\n");
	return item;
}