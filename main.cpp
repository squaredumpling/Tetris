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
    PlayerType type;
    bool dead = false;
    int level=0;
    int score=0;
    int cleared_lines=0;
    bool falling=false;
    Board b;
    int x, y;
    Piece piece;
    Piece next[3];
    int drop_count=0;
    int lastTime=0;
    int difficulty=0;

    bool new_piece = false;
    int ai_rotation=0;
    int ai_position=0;

    struct Textures {
        SDL_Texture *score;
        SDL_Texture *level;
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
                    case SDLK_RETURN: in_title=false; break;
                }
                draw_titlescreen();
            }
            if (event.type == SDL_QUIT) exit(0);
        }
    }

    switch(selected_menu) {
        case 0: PLAYERS = 1; break;
        case 1: PLAYERS = 2; break;
        case 2: exit(0);
    }

    init_game();
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
        bool keypressed = false;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) keypressed = true;
            if (event.type == SDL_QUIT) break;
        }

        int currentTime = SDL_GetTicks();
        for (int i=0; i<PLAYERS; i++)
        {
            switch(p[i].type)
            {
                case P1: if (keypressed && !p[i].dead) process_key(event, SDLK_w, SDLK_d, SDLK_a, SDLK_s, p[i]); break;
                case P2: if (keypressed && !p[i].dead) process_key(event, SDLK_UP, SDLK_RIGHT, SDLK_LEFT, SDLK_DOWN, p[i]); break;
                case AI: if (currentTime > p[i].lastTime && !p[i].dead) {
                    ai_move(p[i]);
                    p[i].lastTime = currentTime;
                }
                break;
            }
        }

        for (int k=0; k<PLAYERS; k++)
            next_frame(p[k]);

        SDL_Delay(16);
    }

    SDL_DestroyWindow( window );
    IMG_Quit();
    SDL_Quit();

}
