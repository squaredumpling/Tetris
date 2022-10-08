#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>

#include "common.h"
#include "pieces.h"
#include "mechanics.h"
#include "ai.h"
#include "graphics.h"
#include "input.h"
using namespace std;

Player p[MAX_PLAYERS];

void init_game_state() {
    // empty boards
    for (int k=0; k<PLAYERS; k++)
        clear_board(p[k]);

    p[0].type = P1;
    p[1].type = AI;

    p[0].difficulty = 0;
    p[1].difficulty = 0;

    // pick random piece and column
    for (int k=0; k<PLAYERS; k++) {
        random_piece(p[k].piece);
        random_piece(p[k].next[0]);
        random_piece(p[k].next[1]);
        random_piece(p[k].next[2]);
        init_crt_piece(p[k]);
    }
}

int main (int argc, char* args[] ) {

    srand(time(NULL));

    init_graphics();

    
    while(true){
        title_input();
        if (selection_input() == 1)
            break;
    }
    

    init_game_state();
    for (int k=0; k<PLAYERS; k++)
        init_player_textures(p[k]);
    
    
    // main game loop
    while (true)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (int k=0; k<PLAYERS; k++)
            print_board(p[k], 4 + k*18, 5);

        SDL_RenderPresent(renderer);

        // read human input
        SDL_Event event;
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
        
        read_game_input(p, PLAYERS);

        for (int k=0; k<PLAYERS; k++)
            next_frame(p[k]);

        SDL_Delay(16);
    }

    SDL_DestroyWindow( window );
    IMG_Quit();
    SDL_Quit();
}
