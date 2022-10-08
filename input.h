//
//  input.h
//  tetris
//
//  Created by Mihnea Buzoiu on 7/27/22.
//

#ifndef input_h
#define input_h

bool title_input();

bool selection_input();

void process_key (SDL_Keycode up, SDL_Keycode rg, SDL_Keycode lf, SDL_Keycode dw, SDL_Keycode fs, Player &p);

void read_game_input(Player p[], const int PLAYERS);


#endif /* input_h */
