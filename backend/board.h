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

// Colors — white pieces are stored as positive numbers, black as negative.
// Multiplying a piece type by the color gives you the
// actual value stored in the board array. e.g. BLACK * QUEEN = -1 * 5 = -5.
#define WHITE  1
#define BLACK -1

// Board layout: board[rank][file]
//   rank 0 = rank 1 (white's back rank)
//   rank 7 = rank 8 (black's back rank)
//   file 0 = 'a', file 7 = 'h'
// Piece encoding: positive = white, negative = black
// e.g. WHITE QUEEN = 5, BLACK QUEEN = -5
typedef struct {
    int squares[8][8];

    int turn; // WHITE or BLACK 
    // Castling rights — set to 1 (allowed) at the start, set to 0 if the king
    // or that rook has moved. Once lost, castling rights can't be recovered.
    int white_castle_k; // Can white still castle kingside (short)?
    int white_castle_q; // Can white still castle queenside (long)?
    int black_castle_k;
    int black_castle_q;

    // En-passant target square — if a pawn just did a double push, this stores
    // the square it skipped over (where an enemy pawn could capture it).
    // Set to -1 if there's no en-passant available this turn.
    int ep_rank;
    int ep_file;

    // Counts moves since the last capture or pawn move (used for 50-move draw rule)
    int halfmove_clock;
    // Counts the number of full moves (increments after Black moves)
    int fullmove_number;
} Board;

void board_init(Board *b);
void copy_board(Board *dst, const Board *src);
int  board_color(int piece);           // WHITE / BLACK / 0
int  board_type(int piece);            // PAWN … KING
void sq_to_str(int rank, int file, char *out);  // -> "e4\0"
int  str_to_rank(const char *sq);      // "e4" -> 3
int  str_to_file(const char *sq);      // "e4" -> 4

#endif // BOARD_H