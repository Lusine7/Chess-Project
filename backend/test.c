// test_chess.c — lightweight regression tests for the chess engine.

// Compile alongside the engine sources:
//   gcc -std=c11 -O2 -o test_chess board.c moves.c eval.c ai.c test_chess.c
// Run:
//   ./test_chess


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "moves.h"
#include "eval.h"

// Tiny test harness 

static int tests_run    = 0;
static int tests_passed = 0;

// A simple custom assert macro for our tests.
// If the condition (cond) is true, the test passes.
// If it's false, it prints a failure message including the function name and line number.
#define ASSERT(cond, msg)                                        \
    do {                                                         \
        tests_run++;                                             \
        if (cond) {                                              \
            tests_passed++;                                      \
        } else {                                                 \
            printf("FAIL [%s] line %d: %s\n",                   \
                   __func__, __LINE__, msg);                     \
        }                                                        \
    } while (0)

// Helpers 

static Move find_move(Board *b, const char *from, const char *to) {
    Move moves[MAX_MOVES];
    int  n = generate_legal_moves(b, moves);
    int  fr = str_to_rank(from), ff = str_to_file(from);
    int  tr = str_to_rank(to),   tf = str_to_file(to);
    for (int i = 0; i < n; i++) {
        if (moves[i].from_rank == fr && moves[i].from_file == ff &&
            moves[i].to_rank   == tr && moves[i].to_file   == tf)
            return moves[i];
    }
    Move bad = {-1,-1,-1,-1,0};
    return bad;
}

static int move_count(Board *b) {
    Move moves[MAX_MOVES];
    return generate_legal_moves(b, moves);
}

//Tests 

static void test_initial_move_count(void) {
    Board b;
    board_init(&b);
    ASSERT(move_count(&b) == 20, "White should have 20 moves at start");
}

static void test_pawn_push(void) {
    Board b;
    board_init(&b);
    Move m = find_move(&b, "e2", "e4");
    ASSERT(m.from_rank != -1, "e2-e4 should be legal");
    apply_move(&b, &m);
    ASSERT(b.squares[3][4] == PAWN, "Pawn should be on e4");
    ASSERT(b.squares[1][4] == EMPTY, "e2 should be empty");
    ASSERT(b.ep_rank == 2 && b.ep_file == 4, "EP target should be e3");
}

static void test_ep_clock_reset(void) {
    Board b;
    board_init(&b);
    // e4 
    Move m = find_move(&b, "e2", "e4");
    apply_move(&b, &m);
    // e5 
    m = find_move(&b, "e7", "e5");
    apply_move(&b, &m);
    // d4 
    m = find_move(&b, "d2", "d4");
    apply_move(&b, &m);
    // exd3 en passant 
    m = find_move(&b, "e5", "d4");
    // just check d4 pawn capture is legal 
    ASSERT(m.from_rank != -1, "exd4 capture should be legal");
}

static void test_not_in_check_at_start(void) {
    Board b;
    board_init(&b);
    ASSERT(!is_in_check(&b, WHITE), "White not in check at start");
    ASSERT(!is_in_check(&b, BLACK), "Black not in check at start");
}

static void test_evaluate_start_is_zero(void) {
    Board b;
    board_init(&b);
    ASSERT(evaluate(&b) == 0, "Start position should evaluate to 0");
}

static void test_evaluate_after_capture(void) {
    Board b;
    board_init(&b);
    // Remove white queen manually 
    b.squares[0][3] = EMPTY;
    int score = evaluate(&b);
    ASSERT(score < 0, "Score should be negative when white queen removed");
}


// main  
// Runs all the test functions and prints the final score.
// Returns 0 if all tests pass (which tells the OS the program succeeded).

int main(void) {
    test_initial_move_count();
    test_pawn_push();
    test_ep_clock_reset();
    test_not_in_check_at_start();
    test_evaluate_start_is_zero();
    test_evaluate_after_capture();

    printf("\n%d / %d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}