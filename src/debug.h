#ifndef DEBUG_H
#define DEBUG_H

#include "definitions.h"

#ifdef DEBUG
void debug_print_board(Board *board);
void debug_print_node(Node *node);
void debug_print_tree(Tree *tree);
void debug_print_list(Movelist *list);
void debug_print_nodelist(Item *list);
#endif

#endif