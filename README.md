# Chess — Final Project README
**ENGS110: Introduction to Programming — Final Project**

This is a chess game built for our Introduction to Programming final project. The backend is written in C and handles all the game logic and AI. The frontend is written in Python using Pygame and handles the graphical interface. They communicate through a subprocess protocol (the Python side launches the C binary and talks to it over stdin/stdout).

---

## Requirements

To build and run this project you need:

- GCC (supporting C11 or later)
- Make
- Python 3.10 or later
- Pygame (Python library)

Install Pygame by running this from the `backend/` folder:

```
make install
```

(This runs `python3 -m pip install pygame` internally, so it works on any system regardless of whether the `pip` or `pip3` command exists.)

---

## How to Build (Compile) the Backend

The C engine has to be compiled before the game can run. The frontend launches it as a subprocess, so the binary needs to exist first.

**Step 1** — open a terminal and go into the backend folder:

```
cd backend
```

**Step 2** — compile with make:

```
make
```

That's it. This runs GCC on all the `.c` source files and produces the chess binary (`chess.exe` on Windows).

If you want to clean up the compiled files and start fresh:

```
make clean
```

---

## How to Run the Game

After compiling the backend, navigate out of backend and run:

```
cd frontend

python main.py   (or python3 main.py on some systems)
```

The game window should open immediately.

---

## How to Use the Application

### Setup Screen

When the game launches, a setup screen appears before any game starts. You pick two things here:

**Your Color:**
- **White** — You move first. The AI plays Black.
- **Black** — The AI moves first. The board is also flipped so your pieces are at the bottom of the screen.

**Difficulty:**
- **Easy** — AI looks 2 moves ahead (fast)
- **Medium** — AI looks 4 moves ahead (default, recommended)
- **Hard** — AI looks 6 moves ahead (slower, stronger)

After choosing, click **Start Game** to begin.

---

### Playing the Game

Everything is mouse-driven:

| Action | Result |
|--------|--------|
| Click one of your pieces | It gets selected. Green dots appear on every square that piece can legally move to. |
| Click a green dot (highlighted square) | The piece moves there. |
| Click the same piece again or anywhere else on the board | Deselects it (cancels the selection). |
| Click a different one of your own pieces | Switches selection to that piece instead. |

The AI automatically makes its move after yours. You don't need to press anything — just wait.

---

### Special Moves

**Castling:**
Click the king, then click the destination square.
- Kingside: `g1` (White) or `g8` (Black)
- Queenside: `c1` (White) or `c8` (Black)

The rook slides over automatically.

**En Passant:**
If en passant is available, the capture square shows up as a green dot automatically. Just click it.

**Pawn Promotion:**
When one of your pawns reaches the last rank, a popup appears asking which piece to promote to. Click one of: Queen, Rook, Bishop, or Knight.

---

### Status Bar

At the bottom of the window there is a status bar that tells you what is happening:

| Message | Meaning |
|---------|---------|
| `White to move · Move 5` | Normal play |
| `White is in CHECK · Move 5` | Your king is in check (also highlighted red) |
| `AI is thinking...` | Wait for the AI |
| `Checkmate! — Black wins` | Game over |
| `Stalemate! — It's a draw` | Game over |
| `Draw! — 50-move rule` | Game over |

---

### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `R` | Restart — goes back to the setup screen so you can start a new game |
| `Q` or `Esc` | Quit the application |

---

## Running the Tests

From inside the `backend/` folder:

Run **only** the C backend unit tests:

```
make test_c
```

Run **only** the Python protocol tests (no binary needed, the protocol tests mock the engine):

```
make test_py
```

Run **all** tests (C + Python) at once:

```
make test
```

All tests should pass before submitting / demoing.

---

## Project Structure

```
backend/
    board.c / board.h     Board representation and piece constants
    moves.c / moves.h     Move generation, legal move filtering,
                          applying moves (including special moves)
    eval.c  / eval.h      Static evaluation — material counting
                          and piece-square tables
    ai.c    / ai.h        Minimax search with alpha-beta pruning
    main.c                The engine entry point — reads commands
                          from stdin and writes responses to stdout
    test_chess.c          C unit tests for the engine
    Makefile              Build system

frontend/
    main.py               Entry point — Pygame window setup,
                          event loop, and setup screen UI
    game.py               Game state machine, move handling,
                          AI thread management
    renderer.py           All drawing — board, pieces, highlights,
                          promotion dialog, status bar
    protocol.py           Subprocess wrapper that talks to the
                          C engine binary over stdin/stdout
    test_protocol.py      Python unit tests for the protocol
                          (mocked, does not need the binary)

    assets/               Chess piece image files (PNGs)
```
