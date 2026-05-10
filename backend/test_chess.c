// test_chess.c - lightweight regression tests for the chess engine.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "moves.h"
#include "eval.h"
#include "ai.h"

static int tests_run    = 0;
static int tests_passed = 0;

void assert_test(int cond, char msg[]) {
    tests_run++;

    if (cond) {
        tests_passed++;
    } else {
        printf("FAIL: %s\n", msg);
    }
}

/* ------------------------------------------------------------------ */
/*  Helpers                                                             */
/* ------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------ */
/*  Original tests                                                      */
/* ------------------------------------------------------------------ */

static void test_initial_move_count(void) {
    Board b;
    board_init(&b);
    assert_test(move_count(&b) == 20, "White should have 20 moves at start");
}

static void test_pawn_push(void) {
    Board b;
    board_init(&b);
    Move m = find_move(&b, "e2", "e4");
    assert_test(m.from_rank != -1, "e2-e4 should be legal");
    apply_move(&b, &m);
    assert_test(b.squares[3][4] == PAWN, "Pawn should be on e4");
    assert_test(b.squares[1][4] == EMPTY, "e2 should be empty");
    assert_test(b.ep_rank == 2 && b.ep_file == 4, "EP target should be e3");
}

static void test_ep_clock_reset(void) {
    Board b;
    board_init(&b);
    /* e4 */
    Move m = find_move(&b, "e2", "e4");
    apply_move(&b, &m);
    /* e5 */
    m = find_move(&b, "e7", "e5");
    apply_move(&b, &m);
    /* d4 */
    m = find_move(&b, "d2", "d4");
    apply_move(&b, &m);
    /* exd4 — just check this pawn capture is legal */
    m = find_move(&b, "e5", "d4");
    assert_test(m.from_rank != -1, "exd4 capture should be legal");
}

static void test_not_in_check_at_start(void) {
    Board b;
    board_init(&b);
    assert_test(!is_in_check(&b, WHITE), "White not in check at start");
    assert_test(!is_in_check(&b, BLACK), "Black not in check at start");
}

static void test_evaluate_start_is_zero(void) {
    Board b;
    board_init(&b);
    assert_test(evaluate(&b) == 0, "Start position should evaluate to 0");
}

static void test_evaluate_after_capture(void) {
    Board b;
    board_init(&b);
    /* Remove white queen manually */
    b.squares[0][3] = EMPTY;
    int score = evaluate(&b);
    assert_test(score < 0, "Score should be negative when white queen removed");
}

/* ------------------------------------------------------------------ */
/*  Coordinate helper tests (sq_to_str, str_to_rank, str_to_file)      */
/* ------------------------------------------------------------------ */

static void test_str_to_rank(void) {
    /* "a1" is rank 0, "a8" is rank 7 */
    assert_test(str_to_rank("a1") == 0, "str_to_rank: a1 should be rank 0");
    assert_test(str_to_rank("a8") == 7, "str_to_rank: a8 should be rank 7");
    assert_test(str_to_rank("e4") == 3, "str_to_rank: e4 should be rank 3");
    assert_test(str_to_rank("h5") == 4, "str_to_rank: h5 should be rank 4");
}

static void test_str_to_file(void) {
    /* "a1" is file 0, "h1" is file 7 */
    assert_test(str_to_file("a1") == 0, "str_to_file: a1 should be file 0");
    assert_test(str_to_file("h1") == 7, "str_to_file: h1 should be file 7");
    assert_test(str_to_file("e4") == 4, "str_to_file: e4 should be file 4");
    assert_test(str_to_file("d7") == 3, "str_to_file: d7 should be file 3");
}

static void test_sq_to_str(void) {
    char buf[3];

    sq_to_str(0, 0, buf);
    assert_test(buf[0] == 'a' && buf[1] == '1', "sq_to_str: rank 0 file 0 should be a1");

    sq_to_str(7, 7, buf);
    assert_test(buf[0] == 'h' && buf[1] == '8', "sq_to_str: rank 7 file 7 should be h8");

    sq_to_str(3, 4, buf);
    assert_test(buf[0] == 'e' && buf[1] == '4', "sq_to_str: rank 3 file 4 should be e4");

    /* The string should always be null-terminated */
    sq_to_str(0, 0, buf);
    assert_test(buf[2] == '\0', "sq_to_str: result should be null-terminated");
}

/* ------------------------------------------------------------------ */
/*  generate_legal_moves in different scenarios                         */
/* ------------------------------------------------------------------ */

static void test_legal_moves_black_start(void) {
    Board b;
    board_init(&b);

    /* After white plays e4, it is black's turn */
    Move m = find_move(&b, "e2", "e4");
    apply_move(&b, &m);

    /* Black should also have 20 moves */
    assert_test(move_count(&b) == 20, "Black should have 20 moves after e4");
}

static void test_legal_moves_knight_opening(void) {
    Board b;
    board_init(&b);

    Move moves[MAX_MOVES];
    int n = generate_legal_moves(&b, moves);

    /* Count how many moves come from a knight */
    int knight_moves = 0;
    for (int i = 0; i < n; i++) {
        int r = moves[i].from_rank;
        int f = moves[i].from_file;
        if (b.squares[r][f] == KNIGHT) {
            knight_moves++;
        }
    }
    /* Each of white's 2 knights can jump to 2 squares = 4 total */
    assert_test(knight_moves == 4, "White should have exactly 4 knight moves at start");
}

static void test_legal_moves_lone_king(void) {
    /* White king alone on e1, black king on e8.
       The white king on e1 can step to d1, d2, e2, f1, f2 = 5 squares. */
    Board b;
    memset(&b, 0, sizeof(b));
    b.turn = WHITE;
    b.ep_rank = -1;
    b.ep_file = -1;
    b.squares[0][4] =  KING;
    b.squares[7][4] = -KING;

    assert_test(move_count(&b) == 5, "Lone king on e1 should have 5 moves");
}

static void test_legal_moves_scholars_mate(void) {
    /* After Scholar's mate (Մանկական մատ) black has 0 legal moves */
    Board b;
    board_init(&b);

    Move m;
    m = find_move(&b, "e2", "e4"); apply_move(&b, &m);
    m = find_move(&b, "e7", "e5"); apply_move(&b, &m);
    m = find_move(&b, "f1", "c4"); apply_move(&b, &m);
    m = find_move(&b, "b8", "c6"); apply_move(&b, &m);
    m = find_move(&b, "d1", "h5"); apply_move(&b, &m);
    m = find_move(&b, "g8", "f6"); apply_move(&b, &m);
    m = find_move(&b, "h5", "f7"); apply_move(&b, &m);

    assert_test(move_count(&b) == 0,        "Scholar's mate: black should have 0 legal moves");
    assert_test(is_in_check(&b, BLACK),     "Scholar's mate: black king should be in check");
}

/* ------------------------------------------------------------------ */
/*  apply_move tests                                                    */
/* ------------------------------------------------------------------ */

static void test_apply_move_turn_changes(void) {
    Board b;
    board_init(&b);

    assert_test(b.turn == WHITE, "Turn should be WHITE before first move");

    Move m = find_move(&b, "e2", "e4");
    apply_move(&b, &m);
    assert_test(b.turn == BLACK, "Turn should switch to BLACK after white moves");

    m = find_move(&b, "e7", "e5");
    apply_move(&b, &m);
    assert_test(b.turn == WHITE, "Turn should switch back to WHITE after black moves");
}

static void test_apply_move_capture(void) {
    /* White pawn on e4 can capture black pawn on d5 */
    Board b;
    memset(&b, 0, sizeof(b));
    b.turn = WHITE;
    b.ep_rank = -1;
    b.ep_file = -1;
    b.squares[0][4] =  KING;
    b.squares[7][4] = -KING;
    b.squares[3][4] =  PAWN;   /* white pawn on e4 */
    b.squares[4][3] = -PAWN;   /* black pawn on d5 */

    Move m = find_move(&b, "e4", "d5");
    assert_test(m.from_rank != -1, "White pawn should be able to capture on d5");

    apply_move(&b, &m);
    assert_test(b.squares[4][3] == PAWN,  "White pawn should be on d5 after capture");
    assert_test(b.squares[3][4] == EMPTY, "e4 should be empty after pawn moved");
}

static void test_apply_move_castling_kingside(void) {
    Board b;
    board_init(&b);

    /* Clear f1 and g1 so kingside castle is available */
    b.squares[0][5] = EMPTY;
    b.squares[0][6] = EMPTY;

    Move m = find_move(&b, "e1", "g1");
    assert_test(m.from_rank != -1, "Kingside castling should be legal when path is clear");

    apply_move(&b, &m);
    assert_test(b.squares[0][6] == KING,  "King should be on g1 after kingside castle");
    assert_test(b.squares[0][5] == ROOK,  "Rook should be on f1 after kingside castle");
    assert_test(b.squares[0][4] == EMPTY, "e1 should be empty after castling");
    assert_test(b.squares[0][7] == EMPTY, "h1 rook should have moved away");
}

/* ------------------------------------------------------------------ */
/*  is_attacked and is_in_check tests                                   */
/* ------------------------------------------------------------------ */

static void test_is_attacked_by_rook(void) {
    Board b;
    memset(&b, 0, sizeof(b));
    b.turn = WHITE;
    b.ep_rank = -1;
    b.ep_file = -1;
    b.squares[0][4] =  KING;
    b.squares[7][4] = -KING;
    b.squares[3][4] =  ROOK;   /* white rook on e4 */

    /* Same file -> e8 (rank 7, file 4) should be attacked */
    assert_test(is_attacked(&b, 7, 4, WHITE), "e8 should be attacked by white rook on e4");
    /* Same rank -> a4 (rank 3, file 0) should be attacked */
    assert_test(is_attacked(&b, 3, 0, WHITE), "a4 should be attacked by white rook on e4");
    /* Diagonal -> d5 should NOT be attacked by a rook */
    assert_test(!is_attacked(&b, 4, 3, WHITE), "d5 should NOT be attacked by a rook on e4");
}

static void test_is_attacked_by_bishop(void) {
    Board b;
    memset(&b, 0, sizeof(b));
    b.turn = WHITE;
    b.ep_rank = -1;
    b.ep_file = -1;
    b.squares[0][4] =  KING;
    b.squares[7][4] = -KING;
    b.squares[3][4] =  BISHOP; /* white bishop on e4 */

    /* h7 is +3 rank +3 file from e4 */
    assert_test(is_attacked(&b, 6, 7, WHITE), "h7 should be attacked by bishop on e4");
    /* b1 is -3 rank -3 file from e4 -> rank 0, file 1 */
    assert_test(is_attacked(&b, 0, 1, WHITE), "b1 should be attacked by bishop on e4");
    /* e5 is straight up, not diagonal */
    assert_test(!is_attacked(&b, 4, 4, WHITE), "e5 should NOT be attacked by bishop on e4");
}

static void test_is_attacked_by_knight(void) {
    Board b;
    memset(&b, 0, sizeof(b));
    b.turn = WHITE;
    b.ep_rank = -1;
    b.ep_file = -1;
    b.squares[0][4] =  KING;
    b.squares[7][4] = -KING;
    b.squares[3][4] =  KNIGHT; /* white knight on e4 */

    /* A knight on e4 can reach d6 (+2 rank, -1 file) and f6 (+2 rank, +1 file) etc. */
    assert_test(is_attacked(&b, 5, 3, WHITE), "d6 should be attacked by knight on e4");
    assert_test(is_attacked(&b, 5, 5, WHITE), "f6 should be attacked by knight on e4");
    /* e5 is not a knight jump */
    assert_test(!is_attacked(&b, 4, 4, WHITE), "e5 should NOT be attacked by a knight on e4");
}

static void test_is_in_check_exposed_king(void) {
    /* White king on e1, black queen on e8, nothing blocking */
    Board b;
    memset(&b, 0, sizeof(b));
    b.turn = WHITE;
    b.ep_rank = -1;
    b.ep_file = -1;
    b.squares[0][4] =  KING;
    b.squares[7][4] = -QUEEN;
    b.squares[7][0] = -KING;

    assert_test(is_in_check(&b, WHITE),  "White king should be in check from black queen on e8");
    assert_test(!is_in_check(&b, BLACK), "Black king on a8 should NOT be in check");
}

static void test_is_in_check_blocked(void) {
    /* Same setup but a white rook on e4 blocks the queen */
    Board b;
    memset(&b, 0, sizeof(b));
    b.turn = WHITE;
    b.ep_rank = -1;
    b.ep_file = -1;
    b.squares[0][4] =  KING;
    b.squares[3][4] =  ROOK;   /* blocks the queen */
    b.squares[7][4] = -QUEEN;
    b.squares[7][0] = -KING;

    assert_test(!is_in_check(&b, WHITE), "White king should NOT be in check when rook blocks");
}

/* ------------------------------------------------------------------ */
/*  evaluate at different positions                                     */
/* ------------------------------------------------------------------ */

static void test_evaluate_extra_rook(void) {
    /* White has an extra rook -> score should be positive */
    Board b;
    board_init(&b);
    b.squares[7][0] = EMPTY;   /* remove black's a-rook */
    assert_test(evaluate(&b) > 0, "Score should be positive when black is missing a rook");
}

static void test_evaluate_symmetric(void) {
    /* Same piece removed from both sides -> still 0 */
    Board b;
    board_init(&b);
    b.squares[0][0] = EMPTY;   /* white a-rook */
    b.squares[7][0] = EMPTY;   /* black a-rook */
    assert_test(evaluate(&b) == 0, "Score should be 0 when same piece removed from both sides");
}

static void test_evaluate_multiple_missing(void) {
    /* White loses queen and both rooks -> score very negative */
    Board b;
    board_init(&b);
    b.squares[0][3] = EMPTY;   /* white queen */
    b.squares[0][0] = EMPTY;   /* white a-rook */
    b.squares[0][7] = EMPTY;   /* white h-rook */
    assert_test(evaluate(&b) < 0, "Score should be very negative when white loses queen and two rooks");
}

/* ------------------------------------------------------------------ */
/*  AI (find_best_move) tests                                           */
/* ------------------------------------------------------------------ */

static void test_ai_returns_a_move(void) {
    Board b;
    board_init(&b);

    Move best;
    int found = find_best_move(&b, &best);

    assert_test(found == 1, "AI should find at least one move at starting position");
    assert_test(best.from_rank >= 0 && best.from_rank <= 7, "AI from_rank should be 0-7");
    assert_test(best.from_file >= 0 && best.from_file <= 7, "AI from_file should be 0-7");
    assert_test(best.to_rank   >= 0 && best.to_rank   <= 7, "AI to_rank should be 0-7");
    assert_test(best.to_file   >= 0 && best.to_file   <= 7, "AI to_file should be 0-7");
}

static void test_ai_takes_free_queen(void) {
    /* White rook on e5, black queen on d5 — the rook can take for free */
    Board b;
    memset(&b, 0, sizeof(b));
    b.turn = WHITE;
    b.ep_rank = -1;
    b.ep_file = -1;
    b.squares[0][4] =  KING;
    b.squares[7][4] = -KING;
    b.squares[4][4] =  ROOK;   /* white rook on e5 */
    b.squares[4][3] = -QUEEN;  /* black queen on d5 */

    ai_depth = 2;
    Move best;
    int found = find_best_move(&b, &best);

    assert_test(found == 1, "AI should find a move when a free queen is available");
    assert_test(best.from_rank == 4 && best.from_file == 4,
                "AI: rook should move from e5 (rank 4, file 4)");
    assert_test(best.to_rank == 4 && best.to_file == 3,
                "AI: rook should capture on d5 (rank 4, file 3)");
}

static void test_ai_no_moves_on_checkmate(void) {
    /* Reach Scholar's mate (Մանկական մատ) then ask AI for black's next move -> returns 0 */
    Board b;
    board_init(&b);

    Move m;
    m = find_move(&b, "e2", "e4"); apply_move(&b, &m);
    m = find_move(&b, "e7", "e5"); apply_move(&b, &m);
    m = find_move(&b, "f1", "c4"); apply_move(&b, &m);
    m = find_move(&b, "b8", "c6"); apply_move(&b, &m);
    m = find_move(&b, "d1", "h5"); apply_move(&b, &m);
    m = find_move(&b, "g8", "f6"); apply_move(&b, &m);
    m = find_move(&b, "h5", "f7"); apply_move(&b, &m);

    Move best;
    int found = find_best_move(&b, &best);
    assert_test(found == 0, "AI should return 0 when the side to move is in checkmate");
}

/* ------------------------------------------------------------------ */
/*  main                                                                */
/* ------------------------------------------------------------------ */

int main(void) {
    // original tests
    test_initial_move_count();
    test_pawn_push();
    test_ep_clock_reset();
    test_not_in_check_at_start();
    test_evaluate_start_is_zero();
    test_evaluate_after_capture();

    // coordinate helpers
    test_str_to_rank();
    test_str_to_file();
    test_sq_to_str();

    // generate_legal_moves 
    test_legal_moves_black_start();
    test_legal_moves_knight_opening();
    test_legal_moves_lone_king();
    test_legal_moves_scholars_mate();

    // apply_move
    test_apply_move_turn_changes();
    test_apply_move_capture();
    test_apply_move_castling_kingside();

    // is_attacked / is_in_check
    test_is_attacked_by_rook();
    test_is_attacked_by_bishop();
    test_is_attacked_by_knight();
    test_is_in_check_exposed_king();
    test_is_in_check_blocked();

    // evaluate
    test_evaluate_extra_rook();
    test_evaluate_symmetric();
    test_evaluate_multiple_missing();

    // AI
    test_ai_returns_a_move();
    test_ai_takes_free_queen();
    test_ai_no_moves_on_checkmate();

    printf("\n%d / %d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}