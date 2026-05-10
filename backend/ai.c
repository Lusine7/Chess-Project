// this is the main part generating the best move for the AI
// it gets the initial depth, then runs the minimax algorithm with alpha-beta pruning with that depth
// it then returns the best move that it found 
// Includes two functions: minimax and find_best_move. find_best_move is called by the frontend, and uses the minimax function.

#include "ai.h"
#include "eval.h"

// Runtime depth — we default to 4 (intermediate). Though via pygame it is later set via DEPTH command. 
int ai_depth = 4;

//  minimax — recursive alpha-beta search                                                                           
//  depth      : plies remaining (0 = leaf, return static evaluation)   
//  alpha      : best score the maximiser can already guarantee        
//  beta       : best score the minimiser can already guarantee        
//  maximizing : 1 if it's White's turn to move at this node       

static int minimax(Board *b, int depth, int alpha, int beta, int maximizing) {

    // Leaf node: return evaluation 
    if (depth == 0)
        return evaluate(b);

    Move moves[MAX_MOVES];
    int  n = generate_legal_moves(b, moves);

    // Terminal node: checkmate or stalemate 
    if (n == 0) {
        if (is_in_check(b, b->turn)) {
            // Checkmate. Return a large score weighted by depth so the
            // engine prefers shorter mates (or delays being mated).
            return maximizing ? -INF + (ai_depth - depth)
                              :  INF - (ai_depth - depth);
        }
        return 0; // Stalemate 
    }

    if (maximizing) {
        int best = -INF;
        for (int i = 0; i < n; i++) {
            Board tmp;
            // Make a copy of the board so we can test the move without changing the real board
            copy_board(&tmp, b);
            apply_move(&tmp, &moves[i]);
            
            // Recursively call minimax for the opponent's turn (they are trying to minimize)
            int score = minimax(&tmp, depth - 1, alpha, beta, 0);
            if (score > best)  best  = score;
            if (score > alpha) alpha = score; // Update alpha (the best score White can guarantee)
            
            // If alpha is greater than or equal to beta, it means Black will never let us reach this position
            // because they have a better alternative already. So we can stop checking the rest of the moves here.
            if (alpha >= beta) break;   // beta cut-off 
        }
        return best;
    } else {
        int best = INF;
        for (int i = 0; i < n; i++) {
            Board tmp;
            // Make a copy of the board so we can test the move without changing the real board
            copy_board(&tmp, b);
            apply_move(&tmp, &moves[i]);
            // Recursively call minimax for our turn (we are trying to maximize)
            int score = minimax(&tmp, depth - 1, alpha, beta, 1);
            if (score < best) best = score;
            if (score < beta) beta = score; // Update beta (the best score Black can guarantee)
            // If alpha is greater than or equal to beta, it means White will never let us reach this position
            // because they have a better alternative already. So we can stop checking the rest of the moves here.
            if (alpha >= beta) break;   // alpha cut-off 
        }
        return best;
    }
}

// find_best_move                                                       
int find_best_move(Board *b, Move *best) {
    Move moves[MAX_MOVES];
    int  n = generate_legal_moves(b, moves);
    if (n == 0) return 0;

    int maximizing  = (b->turn == WHITE);
    int best_score  = maximizing ? -INF : INF;
    *best = moves[0]; // safe default 

    for (int i = 0; i < n; i++) {
        Board tmp;
        copy_board(&tmp, b);
        apply_move(&tmp, &moves[i]);

        int score = minimax(&tmp, ai_depth - 1, -INF, INF, !maximizing);

        if (maximizing ? (score > best_score) : (score < best_score)) {
            best_score = score;
            *best      = moves[i];
        }
    }

    return 1;
}