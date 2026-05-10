"""
main.py — Entry point for the Chess GUI.

Architecture
------------
  SetupScreen — pre-game colour + difficulty picker (this file)
  Renderer    — pure drawing (renderer.py)
  GameState   — logic + protocol bridge (game.py)
  main()      — Pygame event loop

Controls
--------
  Left-click : select piece / move / choose promotion piece
  R          : restart (returns to setup screen)
  Q / Esc    : quit
"""

import sys
import time as _time
import pygame

from renderer import Renderer, STATUS_HEIGHT
from game     import GameState

# Window configuration 
BOARD_SIZE   = 640
WINDOW_W     = BOARD_SIZE
WINDOW_H     = BOARD_SIZE + STATUS_HEIGHT  # used in set_mode and run_setup_screen
FPS          = 60
WINDOW_TITLE = "Chess"


# SetupScreen 
def run_setup_screen(screen: pygame.Surface, clock: pygame.time.Clock):
    # This is a mini "game loop" just for the initial setup menu.
    # It loops until the user clicks "Start Game", returning their choices.
    pygame.display.set_caption("Chess - Setup")
    font = pygame.font.SysFont("sans", 18)

    W  = WINDOW_W
    cx = W // 2

    def make_btn(label, value, x, y, w=160, h=44):
        return {"rect": pygame.Rect(x, y, w, h), "label": label, "value": value, "selected": False}

    # Colour buttons
    color_btns = [
        make_btn("White", "white", cx - 170, 220),
        make_btn("Black", "black", cx + 10,  220),
    ]
    color_btns[0]["selected"] = True

    # Difficulty buttons
    diff_cfg = [("Easy", 2), ("Medium", 4), ("Hard", 6)]
    diff_btns = [
        make_btn(lbl, val, cx - 250 + i * 170, 340)
        for i, (lbl, val) in enumerate(diff_cfg)
    ]
    diff_btns[1]["selected"] = True

    start_rect = pygame.Rect(cx - 90, 440, 180, 48)

    selected_color = "white"
    selected_depth = 4

    def draw_btn(btn):
        bg  = (80, 80, 80) if btn["selected"] else (45, 45, 45)
        pygame.draw.rect(screen, bg, btn["rect"], border_radius=6)
        pygame.draw.rect(screen, (160, 160, 160), btn["rect"], 1, border_radius=6)
        lbl = font.render(btn["label"], True, (230, 230, 230))
        screen.blit(lbl, (btn["rect"].centerx - lbl.get_width() // 2,
                          btn["rect"].centery - lbl.get_height() // 2))

    while True:
        mx, my = pygame.mouse.get_pos()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit(); sys.exit(0)
            if event.type == pygame.KEYDOWN:
                if event.key in (pygame.K_q, pygame.K_ESCAPE):
                    pygame.quit(); sys.exit(0)
            if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                for btn in color_btns:
                    if btn["rect"].collidepoint(mx, my):
                        for b in color_btns: b["selected"] = False
                        btn["selected"] = True
                        selected_color  = btn["value"]
                for btn in diff_btns:
                    if btn["rect"].collidepoint(mx, my):
                        for b in diff_btns: b["selected"] = False
                        btn["selected"] = True
                        selected_depth  = btn["value"]
                if start_rect.collidepoint(mx, my):
                    return selected_color, selected_depth

        # Draw 
        screen.fill((30, 30, 30))

        # Title 
        title = font.render("CHESS", True, (220, 220, 220))
        screen.blit(title, (cx - title.get_width() // 2, 140))

        # Color label
        lbl = font.render("Choose your color:", True, (180, 180, 180))
        screen.blit(lbl, (cx - lbl.get_width() // 2, 190))
        for btn in color_btns:
            draw_btn(btn)

        # Difficulty label
        lbl = font.render("Difficulty:", True, (180, 180, 180))
        screen.blit(lbl, (cx - lbl.get_width() // 2, 310))
        for btn in diff_btns:
            draw_btn(btn)

        # Start button
        bg = (60, 100, 60) if start_rect.collidepoint(mx, my) else (40, 80, 40)
        pygame.draw.rect(screen, bg, start_rect, border_radius=6)
        pygame.draw.rect(screen, (120, 180, 120), start_rect, 1, border_radius=6)
        slbl = font.render("Start Game", True, (220, 220, 220))
        screen.blit(slbl, (start_rect.centerx - slbl.get_width() // 2,
                           start_rect.centery - slbl.get_height() // 2))

        pygame.display.flip()
        clock.tick(FPS)





# main 

def main() -> None:
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_W, WINDOW_H))
    clock  = pygame.time.Clock()

    player_color, depth = run_setup_screen(screen, clock)

    pygame.display.set_caption(WINDOW_TITLE)
    renderer    = Renderer(screen, BOARD_SIZE, flipped=(player_color == "black"))
    game        = GameState(player_color=player_color, depth=depth)
    promo_hover: str | None = None

    # The Main Game Loop! Runs continuously (FPS times a second)
    running = True
    while running:
        # 1. Process Events (mouse clicks, keyboard presses, quitting)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key in (pygame.K_q, pygame.K_ESCAPE):
                    running = False
                elif event.key == pygame.K_r:
                    game.engine.quit()
                    player_color, depth = run_setup_screen(screen, clock)
                    pygame.display.set_caption(WINDOW_TITLE)
                    renderer    = Renderer(screen, BOARD_SIZE,
                                           flipped=(player_color == "black"))
                    game        = GameState(player_color=player_color,
                                            depth=depth)
                    promo_hover = None
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                px, py = event.pos
                if py < BOARD_SIZE:
                    if game.promo_pending:
                        piece = renderer.promo_piece_at(px, py)
                        if piece:
                            game.choose_promotion(piece)
                            promo_hover = None
                    else:
                        sq = renderer.px_to_sq(px, py)
                        if sq:
                            game.click_square(*sq)
            elif event.type == pygame.MOUSEMOTION:
                px, py = event.pos
                if game.promo_pending and py < BOARD_SIZE:
                    promo_hover = renderer.promo_piece_at(px, py)
                else:
                    promo_hover = None

        # 2. Game Logic / AI Trigger
        # If it's the AI's turn, tell it to start thinking in the background
        if (game.needs_ai_move and not game.ai_thinking
                and not game.game_over_msg):
            game.start_ai_move_async()

        # 3. Render graphics
        # Clear the screen with a dark grey color
        screen.fill((20, 20, 20))

        # Little text animation for "AI is thinking..."
        dots        = "." * (int(_time.time() * 2) % 4)
        status_text = (f"AI is thinking{dots}"
                       if game.ai_thinking else game.status_text)

        # Tell the renderer to draw all the visual elements (board, pieces, etc.)
        renderer.draw(
            board_state   = game.board,
            selected_sq   = game.selected_sq,
            legal_dests   = game.legal_dests,
            last_move     = game.last_move,
            in_check_sq   = game.in_check_sq,
            status_text   = status_text,
            turn_color    = game.turn,
            game_over     = game.game_over_msg,
            promo_pending = game.promo_pending,
            promo_hover   = promo_hover,
        )

        pygame.display.flip()
        clock.tick(FPS)

    game.engine.quit()
    pygame.quit()
    sys.exit(0)


if __name__ == "__main__":
    main()