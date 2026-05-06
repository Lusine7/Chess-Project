#ifndef AI_H
#define AI_H

#include "board.h"
#include "moves.h"

#define INF 1000000

extern int ai_depth;

int find_best_move(Board *b, Move *best);

#endif /* AI_H */