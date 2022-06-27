//
//  mechanics.h
//  tetris
//
//  Created by Mihnea Buzoiu on 6/27/22.
//

#ifndef mechanics_h
#define mechanics_h

#include "common.h"

void freeze_piece (int x, int y, Piece piece, Board b);
bool piece_fits (int x, int y, int pc[4][4], Board b);
void copy_piece (Piece from, Piece to);
void rotate_piece (Piece from, Piece to);

int compute_score (int level, int lines);

int clear_lines (Board b);
int focus_clear_lines(Player &p);

void random_piece(Piece &pc);
void init_crt_piece (Player &p);
void next_piece (Player &p);

int drop_speed (Player p);
void p_rotate (Player &p);
void p_right (Player &p);
void p_left (Player &p);
void p_fall (Player &p);
void clear_board (Player &p);
void init_game_state();
void next_frame(Player &p);
bool can_activate_focus(const Player p);
void activate_focus(Player &p);

#endif /* mechanics_h */
