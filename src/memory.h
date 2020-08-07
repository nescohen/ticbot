#ifndef MEMORY_H
#define MEMORY_H

#include "definitions.h"

Movelist *get_movelist();
void free_movelist(Movelist *list);
Board *get_board();
void free_board(Board *board);
Tree *get_tree();
Node *get_node();
void free_node(Node *node);
void free_tree(Tree *tree);
Item *get_item();

#endif