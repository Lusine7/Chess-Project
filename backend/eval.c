#include "eval.h"

// Material values (centipawns)
static const int MAT[7] = {0, 100, 320, 330, 500, 900, 20000};
// P    N    B    R    Q     K 

// Piece-square tables (from White's perspective) 
// These tables tell us how "good" a square is for a specific piece.
// For example, knights are stronger in the center, so their table has higher values there.
// Indexed [rank][file] where rank 0 = White's back rank (rank 1). 
// For Black pieces we mirror the rank: pst_rank = 7 - rank.

static const int PST_PAWN[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    {  5, 10, 10,-20,-20, 10, 10,  5 },
    {  5, -5,-10,  0,  0,-10, -5,  5 },
    {  0,  0,  0, 20, 20,  0,  0,  0 },
    {  5,  5, 10, 25, 25, 10,  5,  5 },
    { 10, 10, 20, 30, 30, 20, 10, 10 },
    { 50, 50, 50, 50, 50, 50, 50, 50 },
    {  0,  0,  0,  0,  0,  0,  0,  0 },
};

static const int PST_KNIGHT[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50 },
    {-40,-20,  0,  0,  0,  0,-20,-40 },
    {-30,  0, 10, 15, 15, 10,  0,-30 },
    {-30,  5, 15, 20, 20, 15,  5,-30 },
    {-30,  0, 15, 20, 20, 15,  0,-30 },
    {-30,  5, 10, 15, 15, 10,  5,-30 },
    {-40,-20,  0,  5,  5,  0,-20,-40 },
    {-50,-40,-30,-30,-30,-30,-40,-50 },
};

static const int PST_BISHOP[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-10,  0,  5, 10, 10,  5,  0,-10 },
    {-10,  5,  5, 10, 10,  5,  5,-10 },
    {-10,  0, 10, 10, 10, 10,  0,-10 },
    {-10, 10, 10, 10, 10, 10, 10,-10 },
    {-10,  5,  0,  0,  0,  0,  5,-10 },
    {-20,-10,-10,-10,-10,-10,-10,-20 },
};

static const int PST_ROOK[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    {  5, 10, 10, 10, 10, 10, 10,  5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    {  0,  0,  0,  5,  5,  0,  0,  0 },
};

static const int PST_QUEEN[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-10,  0,  5,  5,  5,  5,  0,-10 },
    { -5,  0,  5,  5,  5,  5,  0, -5 },
    {  0,  0,  5,  5,  5,  5,  0, -5 },
    {-10,  5,  5,  5,  5,  5,  0,-10 },
    {-10,  0,  5,  0,  0,  0,  0,-10 },
    {-20,-10,-10, -5, -5,-10,-10,-20 },
};

// King is rewarded for being tucked away (castled) in the middlegame 
static const int PST_KING[8][8] = {
    { 20, 30, 10,  0,  0, 10, 30, 20 },
    { 20, 20,  0,  0,  0,  0, 20, 20 },
    {-10,-20,-20,-20,-20,-20,-20,-10 },
    {-20,-30,-30,-40,-40,-30,-30,-20 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
};

// PST lookup
static int pst_value(int type, int rank, int file, int color) {
    // Mirror rank for Black so both sides use the same table orientation
    int r = (color == WHITE) ? rank : (7 - rank);

    switch (type) {
        case PAWN:   return PST_PAWN  [r][file];
        case KNIGHT: return PST_KNIGHT[r][file];
        case BISHOP: return PST_BISHOP[r][file];
        case ROOK:   return PST_ROOK  [r][file];
        case QUEEN:  return PST_QUEEN [r][file];
        case KING:   return PST_KING  [r][file];
        default:     return 0;
    }
}

// evaluate() function
// Evaluates the current board position and returns a score.
// A positive score means White is winning, a negative score means Black is winning.
int evaluate(const Board *b) {
    int score = 0;

    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            int piece = b->squares[r][f];
            if (piece == EMPTY) continue; // Skip empty squares, nothing to evaluate here

            int color = board_color(piece);
            int type  = board_type(piece);
            
            // A piece's value is its base material value (e.g. 900 for Queen)
            // PLUS its positional value from the piece-square table (is it on a good square?)
            int val   = MAT[type] + pst_value(type, r, f, color);

            // White pieces add to score (White wants a higher positive score)
            // Black pieces subtract from score (Black wants a lower negative score)
            // By multiplying with color (1 for White, -1 for Black), we get the correct sign!
            score += color * val;
        }
    }

    return score;
}