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

static void add_pawn_move(Move *moves, int *cnt,
                          int fr, int ff, int tr, int tf, int color) {
    int promo_rank = (color == WHITE) ? 7 : 0;
    if (tr == promo_rank) {
        add_move(moves, cnt, fr, ff, tr, tf, QUEEN);
        add_move(moves, cnt, fr, ff, tr, tf, ROOK);
        add_move(moves, cnt, fr, ff, tr, tf, BISHOP);
        add_move(moves, cnt, fr, ff, tr, tf, KNIGHT);
    } else {
        add_move(moves, cnt, fr, ff, tr, tf, 0);
    }
}

int is_attacked(const Board *b, int rank, int file, int by_color) {
    /* Pawn attacks */
    int pawn_src_rank = (by_color == WHITE) ? rank - 1 : rank + 1;
    for (int df = -1; df <= 1; df += 2) {
        int pf = file + df;
        if (in_bounds(pawn_src_rank, pf) &&
            b->squares[pawn_src_rank][pf] == by_color * PAWN)
            return 1;
    }

    /* Knight attacks */
    static const int kn[8][2] = {
        {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}
    };
    for (int i = 0; i < 8; i++) {
        int nr = rank + kn[i][0], nf = file + kn[i][1];
        if (in_bounds(nr, nf) && b->squares[nr][nf] == by_color * KNIGHT)
            return 1;
    }

    /* Diagonal sliders (bishop / queen) */
    static const int diag[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (int i = 0; i < 4; i++) {
        int r = rank + diag[i][0], f = file + diag[i][1];
        while (in_bounds(r, f)) {
            int p = b->squares[r][f];
            if (p != EMPTY) {
                if (p == by_color * BISHOP || p == by_color * QUEEN) return 1;
                break;
            }
            r += diag[i][0]; f += diag[i][1];
        }
    }

    /* Straight sliders (rook / queen) */
    static const int straight[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (int i = 0; i < 4; i++) {
        int r = rank + straight[i][0], f = file + straight[i][1];
        while (in_bounds(r, f)) {
            int p = b->squares[r][f];
            if (p != EMPTY) {
                if (p == by_color * ROOK || p == by_color * QUEEN) return 1;
                break;
            }
            r += straight[i][0]; f += straight[i][1];
        }
    }

    /* King attacks */
    for (int dr = -1; dr <= 1; dr++) {
        for (int df = -1; df <= 1; df++) {
            if (dr == 0 && df == 0) continue;
            int nr = rank + dr, nf = file + df;
            if (in_bounds(nr, nf) && b->squares[nr][nf] == by_color * KING)
                return 1;
        }
    }

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
    int count = 0;
    int color = b->turn;
    int opp   = -color;

    static const int dirs[8][2] = {
        {1,1},{1,-1},{-1,1},{-1,-1},
        {1,0},{-1,0},{0,1},{0,-1}
    };

    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            int piece = b->squares[r][f];
            if (board_color(piece) != color) continue;
            int type = board_type(piece);

            switch (type) {

            case PAWN: {
                int dir        = color;
                int start_rank = (color == WHITE) ? 1 : 6;
                int nr         = r + dir;

                if (in_bounds(nr, f) && b->squares[nr][f] == EMPTY) {
                    add_pawn_move(moves, &count, r, f, nr, f, color);
                    int nr2 = r + 2 * dir;
                    if (r == start_rank && b->squares[nr2][f] == EMPTY)
                        add_move(moves, &count, r, f, nr2, f, 0);
                }
                for (int df = -1; df <= 1; df += 2) {
                    int nf = f + df;
                    if (!in_bounds(nr, nf)) continue;
                    if (board_color(b->squares[nr][nf]) == opp)
                        add_pawn_move(moves, &count, r, f, nr, nf, color);
                    else if (b->ep_rank == nr && b->ep_file == nf)
                        add_move(moves, &count, r, f, nr, nf, 0);
                }
                break;
            }

            case KNIGHT: {
                static const int kn[8][2] = {
                    {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}
                };
                for (int i = 0; i < 8; i++) {
                    int nr = r + kn[i][0], nf = f + kn[i][1];
                    if (in_bounds(nr, nf) &&
                        board_color(b->squares[nr][nf]) != color)
                        add_move(moves, &count, r, f, nr, nf, 0);
                }
                break;
            }

            case BISHOP:
            case ROOK:
            case QUEEN: {
                int d_start = (type == ROOK)   ? 4 : 0;
                int d_end   = (type == BISHOP)  ? 4 : 8;
                for (int d = d_start; d < d_end; d++) {
                    int nr = r + dirs[d][0], nf = f + dirs[d][1];
                    while (in_bounds(nr, nf)) {
                        int target = b->squares[nr][nf];
                        if (target == EMPTY) {
                            add_move(moves, &count, r, f, nr, nf, 0);
                        } else {
                            if (board_color(target) == opp)
                                add_move(moves, &count, r, f, nr, nf, 0);
                            break;
                        }
                        nr += dirs[d][0]; nf += dirs[d][1];
                    }
                }
                break;
            }

            case KING: {
                for (int dr = -1; dr <= 1; dr++) {
                    for (int df = -1; df <= 1; df++) {
                        if (dr == 0 && df == 0) continue;
                        int nr = r + dr, nf = f + df;
                        if (in_bounds(nr, nf) &&
                            board_color(b->squares[nr][nf]) != color)
                            add_move(moves, &count, r, f, nr, nf, 0);
                    }
                }
                /* castling TODO */
                break;
            }

            }
        }
    }
    return count;
}

void apply_move(Board *b, const Move *m) {
    int piece = b->squares[m->from_rank][m->from_file];
    b->squares[m->to_rank][m->to_file]     = piece;
    b->squares[m->from_rank][m->from_file] = EMPTY;
    if (m->promotion)
        b->squares[m->to_rank][m->to_file] = board_color(piece) * m->promotion;
    b->ep_rank = -1;
    b->ep_file = -1;
    b->turn    = -b->turn;
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