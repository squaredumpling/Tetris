#include <SDL.h>
#include "common.h"
#include "mechanics.h"
#include "ai.h"
#include "graphics.h"


void title_input(){
    bool in_title = true;
    int selected_menu = 0;
    const int buttons_side = 2;
    const int buttons_wide = 2;
    
    draw_titlescreen(selected_menu);
    
    while (in_title){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT: selected_menu += 3; break;
                    case SDLK_RIGHT: selected_menu += 1; break;
                    case SDLK_UP: selected_menu += 2; break;
                    case SDLK_DOWN: selected_menu += 2; break;
                    case SDLK_RETURN: in_title = false; break;
                }
                selected_menu %= (buttons_side * buttons_wide);
                draw_titlescreen(selected_menu);
            }
            if (event.type == SDL_QUIT) exit(0);
        }
        
        SDL_Delay(16);
    }
    
    switch(selected_menu) {
        case 0: PLAYERS = 1; break;
        case 1: PLAYERS = 2; break;
        case 2:
        case 3: exit(0);
    }
}

int selection_input(){
    int in_selection = 0;
    //draw_selectionscreen(selected_menu);
    int ptype[MAX_PLAYERS];
    int pdiff[MAX_PLAYERS];
    
    memset(ptype, 0, MAX_PLAYERS*sizeof(ptype[0]));
    memset(pdiff, 0, MAX_PLAYERS*sizeof(pdiff[0]));
    
    const int MAX_PTYPE = 3;
    const int MAX_PDIFF = 4;
    
    int selcol = 0, selrow = 0;
    draw_selectionscreen(selcol, selrow);
    
    while(in_selection == 0){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT: if(selrow == 0) ptype[selcol] = (ptype[selcol] + 1) % MAX_PTYPE;
                                    else pdiff[selrow] = (pdiff[selrow] + 1) % MAX_PDIFF;
                                    break;
                    case SDLK_RIGHT: if(selrow == 0) ptype[selcol] += (ptype[selcol] + 1) % MAX_PTYPE;
                                     else pdiff[selrow] = (pdiff[selrow] + 1) % MAX_PDIFF;
                                     break;
                    
                    case SDLK_UP: selrow = max(0, selrow-1); break;
                    case SDLK_DOWN: selrow = max(0, selrow+1); break;
                    
                    case SDLK_TAB: selcol = (selcol+1) % MAX_PLAYERS; break;
                    
                    case SDLK_RETURN: in_selection = 1; break;
                    case SDLK_ESCAPE: in_selection = -1; break;
                }
                
                draw_selectionscreen(selcol, selrow);
            }
            if (event.type == SDL_QUIT) exit(0);
        }
        SDL_Delay(16);
    }
    return in_selection;
}

void process_key (SDL_Keycode up, SDL_Keycode rg, SDL_Keycode lf, SDL_Keycode dw, SDL_Keycode fs, Player &p) {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[up]) p_rotate(p);
    if (state[rg]) p_right(p);
    if (state[lf]) p_left(p);
    if (state[dw]) p_fall(p);
    if (state[fs] && can_activate_focus(p)) activate_focus(p);
}

void read_game_input(Player p[], const int PLAYERS) {
    int currentTime = SDL_GetTicks();
    for (int i=0; i<PLAYERS; i++)
    {
        switch(p[i].type)
        {
            case P1:
                if (currentTime > p[i].lastTime + 64 && !p[i].dead) {
                    process_key(SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_R, p[i]);
                    p[i].lastTime = currentTime;
                }
                break;
            case P2:
                if (currentTime > p[i].lastTime + 64 && !p[i].dead) {
                    process_key(SDL_SCANCODE_UP, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_DOWN, SDL_SCANCODE_SLASH, p[i]);
                    p[i].lastTime = currentTime;
                }
                break;
            case AI:
                if (currentTime > p[i].lastTime + 0 && !p[i].dead) {
                    ai_move(p[i]);
                    p[i].lastTime = currentTime;
                }
                break;
        }
    }
}
