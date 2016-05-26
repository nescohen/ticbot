#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define DEBUG
#define ALPHA_BETA
// #define DEBUG_MINIMAX

#define BOARD_SPACES 81
#define BOARD_MACROS 9
#define BOARD_PITCH 9

#define WINNING_BOARDS 8

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
	Move last_move;
	char boards[BOARD_MACROS];
	char spaces[BOARD_SPACES];
};

#endif