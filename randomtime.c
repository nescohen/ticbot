/*******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Adriano Grieb
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 255

struct
{
  int timebank;
  int time_per_move;
  int botid;
} game_settings;

struct
{
  int macro[3 * 3];
  int field[9 * 9];
} board;

void settings(char *, char *);
void update(char *, char *, char *);
void random_action(char *, char *);

int main(int argc, char const *argv[])
{
  char line[MAX_LINE_LENGTH];
  char part[3][MAX_LINE_LENGTH];

  #ifdef DEBUG
  freopen("test.in", "r", stdin);
  #endif
  srand(time(NULL));
  while(fgets(line, MAX_LINE_LENGTH, stdin) != NULL)
  {
    if (!strncmp(line, "action ", 7))
    {
      sscanf(&line[7], "%s %s", part[0], part[1]);
      random_action(part[0], part[1]);
      fflush(stdout);
      continue;
    }
    if (!strncmp(line, "update ", 7))
    {
      sscanf(&line[7], "%s %s %s", part[0], part[1], part[2]);
      update(part[0], part[1], part[2]);
      continue;
    }
    if (!strncmp(line, "settings ", 9))
    {
      sscanf(&line[9], "%s %s", part[0], part[1]);
      settings(part[0], part[1]);
      continue;
    }
  }

  #ifdef DEBUG
  fprintf(stderr, "Game over!\n");
  #endif

  exit(EXIT_SUCCESS);
}

void random_action(char *action, char *value)
{
  assert(action != NULL);
  assert(value != NULL);

  // action move 10000
  if (!strcmp(action, "move"))
  {
    int count = 0;
    int x = 0, y = 0;

    for (int i = 0; i < 9; i++)
    {
      if (board.macro[i] == -1)
      {
        for (int j = 0; j < 9; j++)
        {
          if (board.field[i * 9 + j] == 0)
          {
            count++;
          }
        }
      }
    }

    int pick = rand() % count;
    #ifdef DEBUG
    fprintf(stderr, "available moves: %d pick: %d\n", count, pick);
    #endif

    count = -1;
    for (int i = 0; i < 9; i++)
    {
      if (board.macro[i] == -1)
      {
        for (int j = 0; j < 9; j++)
        {
          if (board.field[i * 9 + j] == 0)
          {
            if (++count == pick)
            {
              x = ((i % 3) * 3) + (j % 3);
              y = ((i / 3) * 3) + (j / 3);
            }
          }
        }
      }
    }

    // place_move 1 1 // places an O (for player 1) in the middle small square of
    //                   the top-left big square
    fprintf(stdout, "place_move %d %d\n", x, y);
    #ifdef DEBUG
    fprintf(stderr, "place_move %d %d\n", x, y);
    #endif
  }
  #ifdef DEBUG
  else
  {
    fprintf(stderr, "unknown action: [%s: %s]\n", action, value);
  }
  #endif
}

void update(char *game, char *update, char *value)
{
  assert(game != NULL);
  assert(update != NULL);
  assert(value != NULL);

  if (strcmp(game, "game"))
  {
    #ifdef DEBUG
    fprintf(stderr, "unknown update: [%s - %s: %s]\n", game, update, value);
    #endif
    return;
  }

  // update game round 1
  if (!strcmp(update, "round"))
  {
    return;
  }
  // update game move 1
  if (!strcmp(update, "move"))
  {
    return;
  }
  // update game macroboard -1,-1,-1,-1,-1,-1,-1,-1,-1
  if (!strcmp(update, "macroboard"))
  {
    sscanf(value, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &board.macro[0],
      &board.macro[1], &board.macro[2], &board.macro[3], &board.macro[4],
      &board.macro[5], &board.macro[6], &board.macro[7], &board.macro[8]);
    return;
  }
  // update game field  0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0,
  //                    0,0,0,0,0,0,0,0,0
  // no new lines and no spaces after the commas
  if (!strcmp(update, "field"))
  {
    sscanf(value,                   // indexes of the vector (ask why? ;)
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 0  1  2  9  10 11 18 19 20
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 3  4  5  12 13 14 21 22 23
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 6  7  8  15 16 17 24 25 26
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 27 28 29 36 37 38 45 46 47
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 30 31 32 39 40 41 48 49 50
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 33 34 35 42 43 44 51 52 53
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 54 55 56 63 64 65 72 73 74
      "%d,%d,%d,%d,%d,%d,%d,%d,%d," // 57 58 59 66 67 68 75 76 77
      "%d,%d,%d,%d,%d,%d,%d,%d,%d", // 60 61 62 69 70 71 78 79 80
      &board.field[0],  &board.field[1],  &board.field[2],  &board.field[9],
      &board.field[10], &board.field[11], &board.field[18], &board.field[19],
      &board.field[20], &board.field[3],  &board.field[4],  &board.field[5],
      &board.field[12], &board.field[13], &board.field[14], &board.field[21],
      &board.field[22], &board.field[23], &board.field[6],  &board.field[7],
      &board.field[8],  &board.field[15], &board.field[16], &board.field[17],
      &board.field[24], &board.field[25], &board.field[26], &board.field[27],
      &board.field[28], &board.field[29], &board.field[36], &board.field[37],
      &board.field[38], &board.field[45], &board.field[46], &board.field[47],
      &board.field[30], &board.field[31], &board.field[32], &board.field[39],
      &board.field[40], &board.field[41], &board.field[48], &board.field[49],
      &board.field[50], &board.field[33], &board.field[34], &board.field[35],
      &board.field[42], &board.field[43], &board.field[44], &board.field[51],
      &board.field[52], &board.field[53], &board.field[54], &board.field[55],
      &board.field[56], &board.field[63], &board.field[64], &board.field[65],
      &board.field[72], &board.field[73], &board.field[74], &board.field[57],
      &board.field[58], &board.field[59], &board.field[66], &board.field[67],
      &board.field[68], &board.field[75], &board.field[76], &board.field[77],
      &board.field[60], &board.field[61], &board.field[62], &board.field[69],
      &board.field[70], &board.field[71], &board.field[78], &board.field[79],
      &board.field[80]);
    return;
  }
}

void settings(char *setting, char *value)
{
  assert(setting != NULL);
  assert(value != NULL);

  // settings timebank 10000
  if (!strcmp(setting, "timebank"))
  {
    game_settings.timebank = atoi(value);
    #ifdef DEBUG
    fprintf(stderr, "settings timebank: %d\n", game_settings.timebank);
    #endif
    return;
  }
  // settings time_per_move 500
  if (!strcmp(setting, "time_per_move"))
  {
    game_settings.time_per_move = atoi(value);
    #ifdef DEBUG
    fprintf(stderr, "time_per_move: %d\n", game_settings.time_per_move);
    #endif
    return;
  }
  // settings player_names player1,player2
  if (!strcmp(setting, "player_names"))
  {
    return;
  }
  // settings your_bot player1
  if (!strcmp(setting, "your_bot"))
  {
    return;
  }
  // settings your_botid 1
  if (!strcmp(setting, "your_botid"))
  {
    game_settings.botid = atoi(value);
    #ifdef DEBUG
    fprintf(stderr, "botid: %d\n", game_settings.botid);
    #endif
    return;
  }

  #ifdef DEBUG
  fprintf(stderr, "unknown setting: [%s: %s]\n", setting, value);
  #endif
}
