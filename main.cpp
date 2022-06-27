#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>


#include "pieces.h"
#include "mechanics.h"
#include "ai.cpp"
#include "graphics.cpp"
#include "input.cpp"
using namespace std;


#define MAX_PLAYERS 10
Player p[MAX_PLAYERS];
int PLAYERS=3;

void init_game_state() {
    // empty boards
    for (int k=0; k<PLAYERS; k++)
        clear_board(p[k]);

    p[0].type = AI;
    p[1].type = AI;
    p[2].type = AI;

    p[0].difficulty = 0;
    p[1].difficulty = 0;
    p[2].difficulty = 0;

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

    // titlescreen
    draw_titlescreen();
    bool in_title=true;
    while (in_title) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT: selected_menu = (selected_menu + 2) % 3; break;
                    case SDLK_RIGHT: selected_menu = (selected_menu + 1) % 3; break;
                    case SDLK_UP: selected_menu = 50; break;
                    case SDLK_DOWN: selected_menu = 0; break;
                    case SDLK_RETURN: in_title = false; break;
                }
                draw_titlescreen();
            }
            if (event.type == SDL_QUIT) exit(0);
        }
        SDL_Delay(16);
    }

    switch(selected_menu) {
        case 0: PLAYERS = 1; break;
        case 1: PLAYERS = 3; break;
        case 2: exit(0);
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
