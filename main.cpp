#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <unistd.h>
#include <math.h>
#include "piece_shapes.h"

using namespace std;

#define MAX_PLAYERS 10

struct Player
{
    int level=1;
    int score=0;
    int cleared_lines=0;
    bool falling=false;
    int b[20][10];
    int x, y;
    int piece[4][4];
    int drop_count=0;
} p[MAX_PLAYERS];




const int SCREEN_WIDTH=1160;
const int SCREEN_HEIGHT=640;
const int PLAYERS=2;



int colors[7][3]={{0x00, 0xAA, 0xAA},
                  {0xAA, 0xAA, 0xAA},
                  {0xAA, 0x00, 0xAA},
                  {0xAA, 0x34, 0x78},
                  {0xAA, 0xAA, 0x44},
                  {0x55, 0xAA, 0xAA},
                  {0xAA, 0x99, 0xAA}};

const int w=20;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* Sans = NULL;

void draw_text(char text[], int x, int y, TTF_Font* font) {

    SDL_Color White = {255, 255, 255};
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, White);

    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(Message, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { x, y, texW, texH };

    SDL_RenderCopy(renderer, Message, NULL, &dstrect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);

}

void print_board (Player p, int xoffset, int yoffset) {

    SDL_SetRenderDrawColor (renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, xoffset*w-1, yoffset*w-1, xoffset*w-1, (22+yoffset)*w);
    SDL_RenderDrawLine(renderer, (xoffset+4)*w-1, yoffset*w-1, (xoffset+4)*w-1, (22+yoffset)*w);
    SDL_RenderDrawLine(renderer, (xoffset+14)*w, yoffset*w-1, (xoffset+14)*w, (22+yoffset)*w);

    SDL_RenderDrawLine(renderer, xoffset*w-1, yoffset*w-1, (xoffset+14)*w-1, yoffset*w-1);
    SDL_RenderDrawLine(renderer, xoffset*w-1, (yoffset+2)*w-1, (xoffset+14)*w-1, (yoffset+2)*w-1);
    SDL_RenderDrawLine(renderer, xoffset*w-1, (yoffset+22)*w, (xoffset+14)*w-1, (yoffset+22)*w);

    char txt[10];
    sprintf(txt, "%9d", p.score);
    draw_text(txt, (xoffset+6)*w, yoffset*w+3, Sans);

    sprintf(txt, "%d", p.level);
    draw_text(txt, (xoffset)*w, yoffset*w+3, Sans);

    for (int i=0; i<20; i++)
    {
        for (int j=0; j<10; j++) {
            SDL_Rect r = {(j+xoffset+4)*w, (i+yoffset+2)*w, w, w};
            if ( i >= p.y && i < p.y+4 &&
                 j >= p.x && j < p.x+4 &&
                 p.piece[i-p.y][j-p.x] != 0)
            {
                // current piece
                int c = p.piece[i-p.y][j-p.x]-1;
                SDL_SetRenderDrawColor(renderer, colors[c][0], colors[c][1], colors[c][2], SDL_ALPHA_OPAQUE);

            }
            else if (p.b[i][j]==0)
            {
                // blank space
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            }
            else {
                // stationary block
                int c = p.b[i][j]-1;
                SDL_SetRenderDrawColor(renderer, colors[c][0], colors[c][1], colors[c][2], SDL_ALPHA_OPAQUE);
            }
            SDL_RenderFillRect(renderer, &r);
        }
    }
}

void freeze_piece (Player &p) {
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (p.piece[i][j] != 0)
                p.b[p.y+i][p.x+j] = p.piece[i][j];
        }
    }
}

bool piece_fits (int x, int y, int pc[4][4], int b[20][10]) {
    bool yes = true;
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if ((pc[i][j] != 0 && b[y+i][x+j] != 0) ||
                (pc[i][j] != 0 && (x+j>=10 || x+j<0)) ||
                (pc[i][j] != 0 && (y+i>=20 || y+i<0)) )
                yes = false;
        }
    }
    return yes;
}

void copy_piece (int from[4][4], int to[4][4]) {
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++){
            to[i][j]=from[i][j];
        }
    }
}

void rotate_piece (int from[4][4], int to[4][4]) {
       for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            to[i][j]=from[j][3-i];
        }
    }
}

int compute_score (int level, int lines) {
    int scores[] = {0, 100, 400, 900, 2000};
    int lev=level;

    if (lev > 8)
        lev = 8;

    return (lev/2 + 1) * scores[lines];
}

int clear_lines (Player &p) {
    int lines=0;
    for (int i=0 ; i<20; i++)
    {
        // check if a row is full
        bool full=true;
        for (int xx = 0 ; xx < 10 && full ; xx++)
        {
            if ( p.b[i][xx] == 0 )
                full=false;
        }

        //clear line
        if ( full )
        {
            for (int yy=i ; yy > 0 ; yy--)
            {
                for (int xx=0 ; xx < 10 ; xx++)
                {
                    p.b[yy][xx] = p.b[yy-1][xx];
                }
            }
            lines++;
        }
    }
    return lines;
}

void random_piece (Player &p) {
    int q = rand() % 7;
    copy_piece(pieces[q], p.piece);
    p.x = 5;
    p.y = 0;
    p.falling = false;
}

void next_piece (Player &p) {
    // freeze piece in last position on the board
    freeze_piece(p);

    // clear lines
    int lines = clear_lines(p);
    p.cleared_lines += lines;

    p.score += compute_score(p.level, lines);
    if (p.cleared_lines >= 5)
    {
        p.level++;
        p.cleared_lines = 0;
    }

    random_piece(p);
}

int drop_speed (Player p) {
    if (p.falling) return 1;
    else return 30.0/sqrt(p.level+1);
}

int main( int argc, char* args[] ) {

    srand(time(NULL));
    rand();

    // empty board
    for (int i=0; i<20; i++){
        for (int j=0; j<10; j++)
            for (int k=0; k<PLAYERS; k++)
                p[k].b[i][j]=0;
    }

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if(TTF_Init()==-1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }
    Sans = TTF_OpenFont("Andale.ttf", 30);

    window = SDL_CreateWindow("", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // pick random piece and column
    for (int k=0; k<PLAYERS; k++)
        random_piece(p[k]);

    // main game loop
    while (true)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {

            if (event.type == SDL_KEYDOWN){

                switch (event.key.keysym.sym)
                {
                    case SDLK_w: {
                        int rot[4][4];
                        rotate_piece(p[0].piece, rot);
                        if (piece_fits(p[0].x, p[0].y, rot, p[0].b))
                            copy_piece(rot, p[0].piece);
                        break;
                    }

                    case SDLK_UP: {
                        int rot[4][4];
                        rotate_piece(p[1].piece, rot);
                        if (piece_fits(p[1].x, p[1].y, rot, p[1].b))
                            copy_piece(rot, p[1].piece);
                        break;
                    }

                    case SDLK_RIGHT: if (piece_fits(p[1].x+1, p[1].y, p[1].piece, p[1].b)) p[1].x++; break;
                    case SDLK_d:       if (piece_fits(p[0].x+1, p[0].y, p[0].piece, p[0].b)) p[0].x++; break;

                    case SDLK_LEFT:  if (piece_fits(p[1].x-1, p[1].y, p[1].piece, p[1].b)) p[1].x--; break;
                    case SDLK_a:       if (piece_fits(p[0].x-1, p[0].y, p[0].piece, p[0].b)) p[0].x--; break;

                    case SDLK_DOWN:  p[1].falling = true; break;
                    case SDLK_s:
                    case SDLK_SPACE:       p[0].falling = true; break;
                }
            }

            else if (event.type == SDL_QUIT) {
                break;
            }
        }



        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (int k=0; k<PLAYERS; k++)
            print_board(p[k], 4 + k*18, 5);

        SDL_RenderPresent(renderer);

        for (int k=0; k<PLAYERS; k++)
        {
            if (!piece_fits(p[k].x, p[k].y+1, p[k].piece, p[k].b)) {
                // game over when new piece can not fit
                if (p[k].y == 0) {
                    
                }

                next_piece(p[k]);
            }

            p[k].drop_count++;
            if (p[k].drop_count >= drop_speed(p[k])) {
                p[k].y++;
                p[k].drop_count = 0;
            }
        }

        SDL_Delay(16);
    }

    SDL_DestroyWindow( window );

    SDL_Quit();

}
