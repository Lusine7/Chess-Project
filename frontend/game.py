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

    # ---------------------------------------------------------------- #
    #  Public API                                                        #
    # ---------------------------------------------------------------- #

    def click_square(self, rank: int, file: int) -> None:
        if self.game_over_msg or self.ai_thinking or self.promo_pending:
            return
        if self.turn != self.player_color:
            return

        piece = self.board[rank][file]

        if self.selected_sq is None:
            if self._is_own_piece(piece):
                self._select(rank, file)
            return

        if (rank, file) == self.selected_sq:
            self._deselect()
            return

        if (rank, file) in self.legal_dests:
            self._try_move(self.selected_sq, (rank, file))
        elif self._is_own_piece(piece):
            self._select(rank, file)
        else:
            self._deselect()

    def choose_promotion(self, piece_char: str) -> None:
        if not self.promo_pending:
            return
        fr, ff = self.promo_from
        tr, tf = self.promo_to
        self._execute_move(fr, ff, tr, tf, promotion=piece_char)
        self.promo_pending = False
        self.promo_from = self.promo_to = None

    # ---------------------------------------------------------------- #
    #  Internal helpers                                                  #
    # ---------------------------------------------------------------- #

    def _is_own_piece(self, piece: str) -> bool:
        if not piece:
            return False
        return piece.isupper() if self.turn == "white" else piece.islower()

    def _select(self, rank: int, file: int) -> None:
        sq_str = self._sq_str(rank, file)
        dests  = self.engine.get_moves(sq_str)
        self.selected_sq = (rank, file)
        self.legal_dests = [self._parse_sq(s) for s in dests]

    def _deselect(self) -> None:
        self.selected_sq = None
        self.legal_dests = []

    def _try_move(self, from_sq: tuple[int, int],
                       to_sq:   tuple[int, int]) -> None:
        fr, ff = from_sq
        tr, tf = to_sq
        piece  = self.board[fr][ff]

        is_promotion = (
            piece in ('P', 'p') and
            ((piece == 'P' and tr == 7) or (piece == 'p' and tr == 0))
        )

        if is_promotion:
            self.promo_pending = True
            self.promo_from    = from_sq
            self.promo_to      = to_sq
            self._deselect()
            return

        self._execute_move(fr, ff, tr, tf)

    def _execute_move(self, fr: int, ff: int, tr: int, tf: int,
                      promotion: Optional[str] = None) -> None:
        from_s = self._sq_str(fr, ff)
        to_s   = self._sq_str(tr, tf)
        ok, _  = self.engine.make_move(from_s, to_s, promotion)
        if not ok:
            self._deselect()
            return

        self._apply_local_move(fr, ff, tr, tf, promotion)
        self.last_move = ((fr, ff), (tr, tf))
        self._deselect()

        # status + AI signal added in next commit
        if self.game_over_msg is None and self.turn != self.player_color:
            self.needs_ai_move = True

    def _apply_local_move(self, fr: int, ff: int, tr: int, tf: int,
                           promo: Optional[str]) -> None:
        piece = self.board[fr][ff]
        ptype = piece.upper() if piece else ''

        # En-passant
        if ptype == 'P' and ff != tf and self.board[tr][tf] == '':
            self.board[fr][tf] = ''

        # Castling rook
        if ptype == 'K' and abs(tf - ff) == 2:
            back = 0 if piece.isupper() else 7
            if tf == 6:
                self.board[back][5] = self.board[back][7]
                self.board[back][7] = ''
            elif tf == 2:
                self.board[back][3] = self.board[back][0]
                self.board[back][0] = ''

        self.board[tr][tf] = piece
        self.board[fr][ff] = ''

        if promo and ptype == 'P':
            promoted = promo.upper() if piece.isupper() else promo.lower()
            self.board[tr][tf] = promoted

    # ---------------------------------------------------------------- #
    #  Utilities                                                         #
    # ---------------------------------------------------------------- #

    @staticmethod
    def _sq_str(rank: int, file: int) -> str:
        return chr(ord('a') + file) + str(rank + 1)

    @staticmethod
    def _parse_sq(sq: str) -> tuple[int, int]:
        file = ord(sq[0]) - ord('a')
        rank = int(sq[1]) - 1
        return rank, file