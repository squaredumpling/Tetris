#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>

typedef int Piece[4][4];
typedef int Board[20][10];

enum PlayerType {P1, P2, AI};

struct Player
{
    // player info
    PlayerType type;
    int difficulty=0;

    // state
    bool falling=false;
    bool dead = false;

    // scoring
    int level=0;
    int score=0;
    int cleared_lines=0;
    int level_cleared_lines=0;
    int focus_cleared_lines=0;
    int drop_count=0;

    // board state
    Board b;
    int x, y;
    Piece piece;
    Piece next[3];

    // ai
    bool new_piece = false;
    int ai_rotation=0;
    int ai_position=0;
    int lastTime=0;

    struct Textures {
        SDL_Texture *score;
        SDL_Texture *level;
        SDL_Texture *cleared_lines;
        SDL_Texture *next_label;
        SDL_Texture *level_label;
        SDL_Texture *ability_label;
        SDL_Texture *player_type_label;
    } txt;
};

#define MAX_PLAYERS 10
Player p[MAX_PLAYERS];
int PLAYERS=3;

#include "piece_characteristics.h"
#include "mechanics.cpp"
#include "ai.cpp"
#include "graphics.cpp"
#include "input.cpp"
using namespace std;

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
        read_game_input();

        for (int k=0; k<PLAYERS; k++)
            next_frame(p[k]);

        SDL_Delay(16);
    }

    SDL_DestroyWindow( window );
    IMG_Quit();
    SDL_Quit();
}
