#include "ai.h"
#include "eval.h"

int ai_depth = 4;  /* start shallow while we test */

static int minimax(Board *b, int depth, int maximizing) {
    if (depth == 0)
        return evaluate(b);

    Move moves[MAX_MOVES];
    int  n = generate_legal_moves(b, moves);

    if (n == 0) {
        if (is_in_check(b, b->turn))
            return maximizing ? -INF : INF;
        return 0; /* stalemate */
    }

    if (maximizing) {
        int best = -INF;
        for (int i = 0; i < n; i++) {
            Board tmp;
            copy_board(&tmp, b);
            apply_move(&tmp, &moves[i]);
            int score = minimax(&tmp, depth - 1, 0);
            if (score > best) best = score;
        }
        return best;
    } else {
        int best = INF;
        for (int i = 0; i < n; i++) {
            Board tmp;
            copy_board(&tmp, b);
            apply_move(&tmp, &moves[i]);
            int score = minimax(&tmp, depth - 1, 1);
            if (score < best) best = score;
        }
        return best;
    }
}

int find_best_move(Board *b, Move *best) {
    Move moves[MAX_MOVES];
    int  n = generate_legal_moves(b, moves);
    if (n == 0) return 0;

    int maximizing = (b->turn == WHITE);
    int best_score = maximizing ? -INF : INF;
    *best = moves[0];

    for (int i = 0; i < n; i++) {
        Board tmp;
        copy_board(&tmp, b);
        apply_move(&tmp, &moves[i]);
        int score = minimax(&tmp, ai_depth - 1, !maximizing);
        if (maximizing ? (score > best_score) : (score < best_score)) {
            best_score = score;
            *best      = moves[i];
        }
    }

    return 1;
}