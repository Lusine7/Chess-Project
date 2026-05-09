========================================================
  CHESS — Build, Run and Usage Guide
========================================================

REQUIREMENTS
------------
  - GCC (C11 or later)
  - Python 3.10 or later
  - Pygame

  Install Pygame:
      pip install pygame


BUILDING THE ENGINE
-------------------
  The C backend must be compiled before the frontend can run.

  From the backend/ directory:

      cd backend
      make

  This produces a chess binary (chess.exe on Windows) inside backend/.

  To verify the build:

      make test

  This runs both the C unit tests and the Python protocol tests.
  All tests should pass before running the game.

  To clean build artifacts:

      make clean


RUNNING THE GAME
----------------
  Once the backend is compiled:

      cd frontend
      python main.py


HOW TO USE
----------

  Setup Screen
  ------------
  When the game launches you choose two settings:

  Your color:
      White — you move first, AI plays Black
      Black — AI moves first, board is flipped so your
              pieces are at the bottom

  Difficulty:
      Easy   — AI searches 2 plies ahead
      Medium — AI searches 4 plies ahead (default)
      Hard   — AI searches 6 plies ahead

  Click Start Game to begin.


  Playing
  -------
  Click a piece            Select it. Legal destination squares
                           are highlighted with green dots.

  Click a highlighted sq   Move the selected piece there.

  Click same piece again   Deselect it.

  Click another own piece  Re-select to that piece.


  Special Moves
  -------------
  Castling      Click the king, then click the destination
                square (g1/g8 kingside, c1/c8 queenside).
                The rook moves automatically.

  En passant    Appears as a legal destination dot
                automatically when available.

  Promotion     When a pawn reaches the back rank a dialog
                appears. Click the desired piece:
                Queen, Rook, Bishop, or Knight.


  Status Bar
  ----------
  The bar at the bottom of the window shows the game state:

      White to move · Move N       Normal play
      White is in CHECK · Move N   King in check (red highlight)
      AI is thinking...            Engine computing its move
      Checkmate! — [Color] wins    Game over
      Stalemate! — It's a draw     Game over
      Draw! — 50-move rule         Game over


  Keyboard Controls
  -----------------
      R        Return to setup screen and start a new game
      Q / Esc  Quit the application


PROJECT STRUCTURE
-----------------
  backend/
      board.c / board.h    Board representation, piece constants
      moves.c / moves.h    Move generation, legal filtering, apply_move
      eval.c  / eval.h     Static evaluation (material + piece-square tables)
      ai.c    / ai.h       Minimax with alpha-beta pruning
      main.c               stdin/stdout command protocol
      test_chess.c         C unit tests
      Makefile

  frontend/
      main.py              Entry point, Pygame event loop, setup screen
      game.py              Game state machine, move logic, AI thread
      renderer.py          Board and UI rendering
      protocol.py          Subprocess wrapper for the C engine
      test_protocol.py     Protocol unit tests (mocked, no binary required)