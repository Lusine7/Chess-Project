#ifndef BOARD_H
#define BOARD_H

#include <string.h>

#define EMPTY  0
#define PAWN   1
#define KNIGHT 2
#define BISHOP 3
#define ROOK   4
#define QUEEN  5
#define KING   6

#define WHITE  1
#define BLACK -1

typedef struct {
    int squares[8][8];
    int turn;

    int white_castle_k;
    int white_castle_q;
    int black_castle_k;
    int black_castle_q;

    int ep_rank;
    int ep_file;

    int halfmove_clock;
    int fullmove_number;
} Board;

void board_init(Board *b);
void copy_board(Board *dst, const Board *src);
int  board_color(int piece);
int  board_type(int piece);
void sq_to_str(int rank, int file, char *out);
int  str_to_rank(const char *sq);
int  str_to_file(const char *sq);

#endif /* BOARD_H */