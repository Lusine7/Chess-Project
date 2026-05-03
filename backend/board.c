#include "board.h"

// Initializes the board with the standard starting position
void board_init(Board *b) {
    // Clear the board
    memset(b->squares, 0, sizeof(b->squares));

    // Setup White pieces (back rank)
    b->squares[0][0] =  ROOK;
    b->squares[0][1] =  KNIGHT;
    b->squares[0][2] =  BISHOP;
    b->squares[0][3] =  QUEEN;
    b->squares[0][4] =  KING;
    b->squares[0][5] =  BISHOP;
    b->squares[0][6] =  KNIGHT;
    b->squares[0][7] =  ROOK;

    // Setup White pawns
    for (int f = 0; f < 8; f++)
        b->squares[1][f] = PAWN;

    // Setup Black pawns
    for (int f = 0; f < 8; f++)
        b->squares[6][f] = -PAWN;

    // Setup Black pieces (back rank)
    b->squares[7][0] = -ROOK;
    b->squares[7][1] = -KNIGHT;
    b->squares[7][2] = -BISHOP;
    b->squares[7][3] = -QUEEN;
    b->squares[7][4] = -KING;
    b->squares[7][5] = -BISHOP;
    b->squares[7][6] = -KNIGHT;
    b->squares[7][7] = -ROOK;

    // White always starts first
    b->turn = WHITE;
}