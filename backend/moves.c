#include "moves.h"
#include <stdlib.h>

static int in_bounds(int rank, int file) {
    return rank >= 0 && rank < 8 && file >= 0 && file < 8;
}

static void add_move(Move *moves, int *cnt,
                     int fr, int ff, int tr, int tf, int promo) {
    moves[*cnt].from_rank = fr;
    moves[*cnt].from_file = ff;
    moves[*cnt].to_rank   = tr;
    moves[*cnt].to_file   = tf;
    moves[*cnt].promotion = promo;
    (*cnt)++;
}

int is_attacked(const Board *b, int rank, int file, int by_color) {
    /* TODO */
    (void)b; (void)rank; (void)file; (void)by_color;
    return 0;
}

int is_in_check(const Board *b, int color) {
    for (int r = 0; r < 8; r++)
        for (int f = 0; f < 8; f++)
            if (b->squares[r][f] == color * KING)
                return is_attacked(b, r, f, -color);
    return 0;
}

int generate_pseudo_moves(const Board *b, Move *moves) {
    /* TODO */
    (void)b; (void)moves;
    return 0;
}

void apply_move(Board *b, const Move *m) {
    /* TODO */
    (void)b; (void)m;
}

int generate_legal_moves(Board *b, Move *moves) {
    Move pseudo[MAX_MOVES];
    int  n     = generate_pseudo_moves(b, pseudo);
    int  count = 0;
    for (int i = 0; i < n; i++) {
        Board tmp;
        copy_board(&tmp, b);
        apply_move(&tmp, &pseudo[i]);
        if (!is_in_check(&tmp, b->turn))
            moves[count++] = pseudo[i];
    }
    return count;
}