#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "moves.h"
#include "ai.h"

static Board game;

// INIT
// Resets the board to the starting position and confirms it's ready.
static void handle_init(void) {
    board_init(&game);
    printf("OK\n");
    fflush(stdout); // Always flush stdout so the Python frontend sees the reply immediately!
}

// MOVES <sq> - return legal destination squares for a piece
static void handle_moves(const char *sq) {
    if (!sq || strlen(sq) < 2) {
        printf("ERROR invalid square\n");
        fflush(stdout);
        return;
    }

    int rank = str_to_rank(sq);
    int file = str_to_file(sq);

    if (rank < 0 || rank > 7 || file < 0 || file > 7) {
        printf("ERROR invalid square\n");
        fflush(stdout);
        return;
    }

    Move moves[MAX_MOVES];
    int  n = generate_legal_moves(&game, moves);

    printf("MOVES");
    for (int i = 0; i < n; i++) {
        if (moves[i].from_rank == rank && moves[i].from_file == file) {
            char dest[3];
            sq_to_str(moves[i].to_rank, moves[i].to_file, dest);
            printf(" %s", dest);
        }
    }
    printf("\n");
    fflush(stdout);
}

// MOVE <from><to>
static void handle_move(const char *arg) {
    if (!arg || strlen(arg) < 4) {
        printf("ERROR invalid move format\n");
        fflush(stdout);
        return;
    }

    int fr = str_to_rank(arg);
    int ff = str_to_file(arg);
    int tr = str_to_rank(arg + 2);
    int tf = str_to_file(arg + 2);

    // Optional promotion character
    int promo = 0;
    if (strlen(arg) >= 5) {
        switch (arg[4]) {
            case 'q': promo = QUEEN;  break;
            case 'r': promo = ROOK;   break;
            case 'b': promo = BISHOP; break;
            case 'n': promo = KNIGHT; break;
        }
    }

    Move moves[MAX_MOVES];
    int  n = generate_legal_moves(&game, moves);

    for (int i = 0; i < n; i++) {
        Move *m = &moves[i];
        if (m->from_rank != fr || m->from_file != ff ||
            m->to_rank   != tr || m->to_file   != tf)
            continue;

        // Promotion matching:
        // - If the move needs a promotion piece (e.g. pawn reaches back rank),
        //   use whatever the player picked, defaulting to Queen if they didn't specify.
        // - If the move doesn't involve promotion, make sure we're not accidentally
        //   matching a promotion variant of the same from/to squares.
        if (m->promotion != 0) {
            int want = (promo != 0) ? promo : QUEEN; // default to queen if unspecified
            if (m->promotion != want) continue;
        } else {
            if (promo != 0) continue; // this move doesn't promote, skip if caller asked for promo
        }

        apply_move(&game, m);

        char from_s[3], to_s[3];
        sq_to_str(fr, ff, from_s);
        sq_to_str(tr, tf, to_s);
        printf("MOVED %s%s\n", from_s, to_s);
        fflush(stdout);
        return;
    }

    printf("ERROR illegal move\n");
    fflush(stdout);
}

// DEPTH <n> - set runtime search depth (2 / 4 / 6)
static void handle_depth(const char *arg) {
    if (!arg) {
        printf("ERROR missing depth\n");
        fflush(stdout);
        return;
    }
    int d = atoi(arg);
    if (d < 1 || d > 7) {
        printf("ERROR depth out of range\n");
        fflush(stdout);
        return;
    }
    ai_depth = d;
    printf("OK\n");
    fflush(stdout);
}

// AI_MOVE - engine picks and plays the best move
static void handle_ai_move(void) {
    Move best;
    if (!find_best_move(&game, &best)) {
        printf("ERROR no moves available\n");
        fflush(stdout);
        return;
    }

    char from_s[3], to_s[3];
    sq_to_str(best.from_rank, best.from_file, from_s);
    sq_to_str(best.to_rank,   best.to_file,   to_s);

    apply_move(&game, &best);

    if (best.promotion) {
        char pc = '?';
        switch (best.promotion) {
            case QUEEN:  pc = 'q'; break;
            case ROOK:   pc = 'r'; break;
            case BISHOP: pc = 'b'; break;
            case KNIGHT: pc = 'n'; break;
        }
        printf("AI_MOVED %s%s%c\n", from_s, to_s, pc);
    } else {
        printf("AI_MOVED %s%s\n", from_s, to_s);
    }
    fflush(stdout);
}

// STATUS
// Asks the engine what's happening in the game right now.
// It checks if there are any legal moves, and whether the king is in check,
// to figure out if we're playing normally, in check, checkmated, or stalemated.
static void handle_status(void) {
    const char *turn = (game.turn == WHITE) ? "white" : "black";

    Move moves[MAX_MOVES];
    int  n = generate_legal_moves(&game, moves);

    const char *state;
    if (n == 0) {
        // No legal moves left: if the king is in check, it's checkmate. Otherwise stalemate.
        state = is_in_check(&game, game.turn) ? "checkmate" : "stalemate";
    } else if (is_in_check(&game, game.turn)) {
        state = "check"; // Has moves, but king is currently under attack
    } else if (game.halfmove_clock >= 100) {
        // 100 halfmoves = 50 full moves without a pawn push or capture = draw
        state = "draw";
    } else {
        state = "playing";
    }

    printf("STATUS %s %s\n", turn, state);
    fflush(stdout);
}

// main - read-dispatch loop
// This is the entry point for the C engine. It runs in an infinite loop,
// waiting for text commands from the standard input (stdin) sent by the Python frontend.
int main(void) {
    board_init(&game);

    char line[256];
    // fgets blocks (waits) until a new line of text comes in
    while (fgets(line, sizeof(line), stdin)) {
        // Strip trailing newline / carriage-return so we just have the raw command
        int len = (int)strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';

        // Dispatch to the correct handler based on the first word.
        // "line + 6" means skip the first 6 characters (e.g. "DEPTH " is 6 chars)
        // so we pass only the argument part to the handler function.
        if      (strcmp(line, "INIT")        == 0) handle_init();
        else if (strncmp(line, "DEPTH ",  6) == 0) handle_depth(line + 6);  // passes e.g. "4"
        else if (strncmp(line, "MOVES ",  6) == 0) handle_moves(line + 6);  // passes e.g. "e2"
        else if (strncmp(line, "MOVE ",   5) == 0) handle_move(line + 5);   // passes e.g. "e2e4"
        else if (strcmp(line, "AI_MOVE")     == 0) handle_ai_move();
        else if (strcmp(line, "STATUS")      == 0) handle_status();
        else if (strcmp(line, "QUIT")        == 0) break; // Exit the loop and close the engine
        else {
            printf("ERROR unknown command\n");
            fflush(stdout);
        }
    }

    return 0;
}