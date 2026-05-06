#include "eval.h"

/* Material values (centipawns) */
static const int MAT[7] = {0, 100, 320, 330, 500, 900, 20000};
/*                            P    N    B    R    Q     K   */

int evaluate(const Board *b) {
    int score = 0;

    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            int piece = b->squares[r][f];
            if (piece == EMPTY) continue;

            int color = board_color(piece);
            int type  = board_type(piece);

            score += color * MAT[type];
        }
    }

    return score;
}