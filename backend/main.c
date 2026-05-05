#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "moves.h"

static Board game;

static void handle_init(void) {
    board_init(&game);
    printf("OK\n");
    fflush(stdout);
}

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

static void handle_move(const char *arg) {
    if (!arg || strlen(arg) < 4) {
        printf("ERROR invalid move format\n");
        fflush(stdout);
        return;
    }
    int fr = str_to_rank(arg),      ff = str_to_file(arg);
    int tr = str_to_rank(arg + 2),  tf = str_to_file(arg + 2);
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
        if (m->promotion != 0) {
            int want = (promo != 0) ? promo : QUEEN;
            if (m->promotion != want) continue;
        } else {
            if (promo != 0) continue;
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

static void handle_status(void) {
    const char *turn = (game.turn == WHITE) ? "white" : "black";
    Move moves[MAX_MOVES];
    int  n = generate_legal_moves(&game, moves);
    const char *state;
    if (n == 0)
        state = is_in_check(&game, game.turn) ? "checkmate" : "stalemate";
    else if (is_in_check(&game, game.turn))
        state = "check";
    else if (game.halfmove_clock >= 100)
        state = "draw";
    else
        state = "playing";
    printf("STATUS %s %s\n", turn, state);
    fflush(stdout);
}

int main(void) {
    board_init(&game);
    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        int len = (int)strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';
        if      (strcmp(line, "INIT")       == 0) handle_init();
        else if (strncmp(line, "MOVES ", 6) == 0) handle_moves(line + 6);
        else if (strncmp(line, "MOVE ",  5) == 0) handle_move(line + 5);
        else if (strcmp(line, "STATUS")     == 0) handle_status();
        else if (strcmp(line, "QUIT")       == 0) break;
        else {
            printf("ERROR unknown command\n");
            fflush(stdout);
        }
    }
    return 0;
}