//
//  graphics.h
//  tetris
//
//  Created by Mihnea Buzoiu on 7/27/22.
//

#ifndef graphics_h
#define graphics_h

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

int max(const int a, const int b);

void init_graphics();

void create_text(SDL_Texture *&t, const char text[], TTF_Font* font, unsigned char r, unsigned char g, unsigned char b);

void render_text(int x, int y, SDL_Texture *txt);

void init_player_textures(Player &p);

int draw_rectangle(int x1, int y1, int x2, int y2);

void draw_gradient_line(int x, int y, int l);

void draw_indicators(Player p, int xoffset, int yoffset);

void print_board (Player &p, int xoffset, int yoffset);

void draw_titlescreen(int selected_menu);

void draw_selectionscreen(int col, int row);

extern SDL_Window* window;
extern SDL_Renderer* renderer;

#endif /* graphics_h */
