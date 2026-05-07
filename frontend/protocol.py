"""
protocol.py — thin wrapper around the C chess engine subprocess.

All communication is line-based ASCII over stdin / stdout.
Every method blocks until the engine responds (one line).
"""

import subprocess
import os


class ChessProtocol:
    def __init__(self, binary_path: str):
        if not os.path.exists(binary_path):
            raise FileNotFoundError(
                f"Chess binary not found at '{binary_path}'.\n"
                "Run:  cd backend && make"
            )
        self.proc = subprocess.Popen(
            [binary_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
            bufsize=1,
        )

    def _send(self, command: str) -> str:
        self.proc.stdin.write(command + "\n")
        self.proc.stdin.flush()
        reply = self.proc.stdout.readline()
        return reply.strip()

    def init(self) -> bool:
        return self._send("INIT") == "OK"

    def set_depth(self, depth: int) -> bool:
        return self._send(f"DEPTH {depth}") == "OK"

    def get_moves(self, square: str) -> list[str]:
        reply = self._send(f"MOVES {square}")
        if reply.startswith("MOVES"):
            parts = reply.split()
            return parts[1:]
        return []

    def make_move(self, from_sq: str, to_sq: str,
                  promotion: str | None = None) -> tuple[bool, str]:
        move_str = from_sq + to_sq + (promotion or "")
        reply = self._send(f"MOVE {move_str}")
        return reply.startswith("MOVED"), reply

    def ai_move(self) -> tuple[bool, str | None, str | None, str | None]:
        reply = self._send("AI_MOVE")
        if reply.startswith("AI_MOVED"):
            parts = reply.split()
            move  = parts[1]
            from_sq = move[:2]
            to_sq   = move[2:4]
            promo   = move[4] if len(move) > 4 else None
            return True, from_sq, to_sq, promo
        return False, None, None, None

    def status(self) -> tuple[str | None, str | None]:
        reply = self._send("STATUS")
        parts = reply.split()
        if len(parts) == 3 and parts[0] == "STATUS":
            return parts[1], parts[2]
        return None, None

    def quit(self) -> None:
        try:
            self._send("QUIT")
        except Exception:
            pass
        try:
            self.proc.terminate()
        except Exception:
            pass