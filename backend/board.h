#ifndef BOARD_H
#define BOARD_H

#include <string.h>

// Piece types
#define EMPTY  0
#define PAWN   1
#define KNIGHT 2
#define BISHOP 3
#define ROOK   4
#define QUEEN  5
#define KING   6

// Colors
#define WHITE  1
#define BLACK -1

// Structure representing the chess board state
typedef struct {
    int squares[8][8]; // 2D array of squares: positive for White, negative for Black
    int turn;          // Whose turn it is: WHITE or BLACK
} Board;

// Initializes the board with the starting position
void board_init(Board *b);

#endif /* BOARD_H */