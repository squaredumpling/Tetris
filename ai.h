//
//  ai.h
//  tetris
//
//  Created by Mihnea Buzoiu on 7/27/22.
//

#ifndef ai_h
#define ai_h

int evaluate_board (Board b);
void compute_next_piece (Player &p);
void ai_move (Player &p);

#endif /* ai_h */
