#ifndef MOVES_H
#define MOVES_H

#include "board.h"

// 256 is more than enough for any chess position (theoretical max is around 218)
#define MAX_MOVES 256

// A single move: where the piece starts, where it goes, and what it promotes to.
// `promotion` is 0 for normal moves, or a piece type (QUEEN, ROOK, etc.) for pawn promotions.
typedef struct {
    int from_rank, from_file; // Starting square
    int to_rank,   to_file;   // Destination square
    int promotion; // 0 for normal moves, or QUEEN / ROOK / BISHOP / KNIGHT when a pawn promotes
} Move;

// Attack & check queries 
int is_attacked(const Board *b, int rank, int file, int by_color);
int is_in_check(const Board *b, int color);

// Move application (modifies board in-place) 
void apply_move(Board *b, const Move *m);

// Move generation 
int generate_pseudo_moves(const Board *b, Move *moves); //ignores check  
int generate_legal_moves(Board *b, Move *moves); //fully legal    

#endif //MOVES_H 