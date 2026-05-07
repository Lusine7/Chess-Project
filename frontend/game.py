"""
game.py — Chess game logic / state machine.
"""

from __future__ import annotations
from protocol import ChessProtocol
from typing   import Optional
import os

_HERE    = os.path.dirname(os.path.abspath(__file__))
_BACKEND = os.path.join(_HERE, "..", "backend")

_BINARY_WIN  = os.path.join(_BACKEND, "chess.exe")
_BINARY_UNIX = os.path.join(_BACKEND, "chess")
BINARY_PATH  = _BINARY_WIN if os.path.exists(_BINARY_WIN) else _BINARY_UNIX


def _starting_board() -> list[list[str]]:
    """Return an 8x8 grid of piece chars (rank 0 = white's back rank)."""
    b: list[list[str]] = [[""] * 8 for _ in range(8)]
    back = ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']
    for f, p in enumerate(back):
        b[0][f] = p
        b[7][f] = p.lower()
    for f in range(8):
        b[1][f] = 'P'
        b[6][f] = 'p'
    return b


class GameState:
    def __init__(self, player_color: str = "white", depth: int = 4):
        self.player_color = player_color
        self.engine       = ChessProtocol(BINARY_PATH)
        self.engine.init()
        self.engine.set_depth(depth)

        self.board: list[list[str]] = _starting_board()

        self.selected_sq:  Optional[tuple[int, int]] = None
        self.legal_dests:  list[tuple[int, int]]     = []
        self.last_move:    Optional[tuple[tuple[int, int], tuple[int, int]]] = None
        self.in_check_sq:  Optional[tuple[int, int]] = None

        self.turn:          str = "white"
        self.move_number:   int = 1
        self.status_text:   str = "White to move"
        self.game_over_msg: Optional[str] = None

        self.promo_pending: bool                      = False
        self.promo_from:    Optional[tuple[int, int]] = None
        self.promo_to:      Optional[tuple[int, int]] = None

        self.ai_thinking:   bool = False
        self.needs_ai_move: bool = (player_color == "black")

    @staticmethod
    def _sq_str(rank: int, file: int) -> str:
        return chr(ord('a') + file) + str(rank + 1)

    @staticmethod
    def _parse_sq(sq: str) -> tuple[int, int]:
        file = ord(sq[0]) - ord('a')
        rank = int(sq[1]) - 1
        return rank, file

    def _is_own_piece(self, piece: str) -> bool:
        if not piece:
            return False
        return piece.isupper() if self.turn == "white" else piece.islower()