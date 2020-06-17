#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <unistd.h>
#include <math.h>
#include "piece_characteristics.h"

using namespace std;

#define MAX_PLAYERS 10

enum PlayerType {P1, P2, AI};

typedef int Piece[4][4];
typedef int Board[20][10];

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
} p[MAX_PLAYERS];




const int SCREEN_WIDTH=1160;
const int SCREEN_HEIGHT=640;
const int PLAYERS=3;
const int DIFFICULTY=2;



int colors[][3]={{0, 0, 0},
                 {0, 0, 255},
                 {255, 255, 0},
                 {255, 0, 255},
                 {0, 255, 255},
                 {255, 128, 0},
                 {0, 255, 0},
                 {255, 0, 0}};


const int w = 20;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* Sans = NULL;
TTF_Font* Small = NULL;

void draw_text(const char text[], int x, int y, TTF_Font* font, unsigned char r, unsigned char g, unsigned char b) {

    SDL_Color color_t = {r, g, b};
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color_t);

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

    char txt[10]; // text holder

    SDL_SetRenderDrawColor (renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    // big border vertical
    SDL_RenderDrawLine(renderer, xoffset*w-1, yoffset*w-1, xoffset*w-1, (22+yoffset)*w);
    SDL_RenderDrawLine(renderer, (xoffset+4)*w-1, yoffset*w-1, (xoffset+4)*w-1, (22+yoffset)*w);
    SDL_RenderDrawLine(renderer, (xoffset+14)*w, yoffset*w-1, (xoffset+14)*w, (22+yoffset)*w);
    //big border horizontal
    SDL_RenderDrawLine(renderer, xoffset*w-1, yoffset*w-1, (xoffset+14)*w-1, yoffset*w-1);
    SDL_RenderDrawLine(renderer, xoffset*w-1, (yoffset+2)*w-1, (xoffset+14)*w-1, (yoffset+2)*w-1);
    SDL_RenderDrawLine(renderer, xoffset*w-1, (yoffset+22)*w, (xoffset+14)*w-1, (yoffset+22)*w);
    //small square 1
    SDL_Rect s1 = {xoffset*w-1+w/2, (yoffset+2)*w-1+w/2, w*3+2, w*3+2+w/2};
    SDL_RenderDrawRect(renderer, &s1);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2, (yoffset+3)*w-1+w/2, (xoffset+3)*w-1+w/2, (yoffset+3)*w-1+w/2);
    // small square 2
    SDL_Rect s2 = {xoffset*w-1+w/2, (yoffset+7)*w-1, w*3+2, w*3+2+w/2};
    SDL_RenderDrawRect(renderer, &s2);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2, (yoffset+8)*w-1, (xoffset+3)*w-1+w/2, (yoffset+8)*w-1);
    // next piece rectangle
    SDL_Rect n = {xoffset*w-1+w/2, (yoffset+11)*w-1+w/2, w*3+2, 10*w};
    SDL_RenderDrawRect(renderer, &n);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2, (yoffset+12)*w + w/2, (xoffset+3)*w-1 + w/2, (yoffset+12)*w + w/2);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2, (yoffset+15)*w + w/2, (xoffset+3)*w-1 + w/2, (yoffset+15)*w + w/2);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2, (yoffset+18)*w + w/2, (xoffset+3)*w-1 + w/2, (yoffset+18)*w + w/2);

    sprintf(txt, "%7d", p.score);
    draw_text(txt, (xoffset+6)*w-10, yoffset*w+3, Sans, 255, 255, 255);

    if (p.level < 8) {
        sprintf(txt, "%d", p.level);
        draw_text(txt, (xoffset+1)*w + w/2, (yoffset+4)*w - w/4, Sans, 255, 255, 255);
    }
    else {
        draw_text("8+", (xoffset+1)*w, (yoffset+4)*w - w/4, Sans, 255, 255, 255);
    }

    int red=0, green=0, blue=0;
    switch(p.type) {
        case P1: sprintf(txt, "P1"); red=200; green=60; blue=200; break;
        case P2: sprintf(txt, "P2"); red=200; green=60; blue=200; break;
        case AI: sprintf(txt, "AI"); red=60; green=200; blue=60; break;
    }

    draw_text(txt, (xoffset+1)*w, yoffset*w, Sans, red, green, blue);
    draw_text("LEVEL", (xoffset+1)*w, (yoffset+2)*w + w/2 + 2, Small, 255, 255, 255);
    draw_text("ABILITY", xoffset*w + w/2 + 2, (yoffset+7)*w + 2, Small, 255, 255, 255);
    draw_text("NEXT", xoffset*w + w + 4, (yoffset+11)*w + w/2 + 2, Small, 255, 255, 255);

    // draw board pieces
    for (int i=0; i<20; i++)
    {
        for (int j=0; j<10; j++) {
            SDL_Rect r = {(j+xoffset+4)*w, (i+yoffset+2)*w, w, w};
            if ( i >= p.y && i < p.y+4 &&
                 j >= p.x && j < p.x+4 &&
                 p.piece[i-p.y][j-p.x] != 0)
            {
                // current piece
                int c = p.piece[i-p.y][j-p.x];
                SDL_SetRenderDrawColor(renderer, colors[c][0], colors[c][1], colors[c][2], SDL_ALPHA_OPAQUE);

            }
            else {
                // stationary block
                int c = p.b[i][j];
                SDL_SetRenderDrawColor(renderer, colors[c][0], colors[c][1], colors[c][2], SDL_ALPHA_OPAQUE);
            }
            SDL_RenderFillRect(renderer, &r);
        }
    }

    // draw next pieces
    int dx = (xoffset+1) * w;
    int dy = (yoffset+13) * w;
    for (int k=0; k < 3 - DIFFICULTY; k++) {
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                SDL_Rect r = { dx + j*(w/2), dy + ((k*4)+i)*(w/2) + k*w, w/2, w/2};
                int c = p.next[k][i][j];
                SDL_SetRenderDrawColor(renderer, colors[c][0], colors[c][1], colors[c][2], SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }

    SDL_SetRenderDrawColor (renderer, 200, 0, 100, SDL_ALPHA_OPAQUE);
    int width=12;
    for (int k= max(3-DIFFICULTY, 0); k<3; k++) {
        for (int i=0; i<width; i++) {
            SDL_RenderDrawLine (renderer, dx, dy + k*3*w + i, dx + 2*w, dy + (k*3+2)*w + i - width);
            SDL_RenderDrawLine (renderer, dx, dy + (k*3+2)*w + i - width, dx + 2*w, dy + k*3*w +i);
        }
    }
}

void freeze_piece (int x, int y, Piece piece, Board b) {
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (piece[i][j] != 0)
                b[y+i][x+j] = piece[i][j];
        }
    }
}

bool piece_fits (int x, int y, int pc[4][4], Board b) {
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

void copy_piece (Piece from, Piece to) {
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++){
            to[i][j]=from[i][j];
        }
    }
}

void rotate_piece (Piece from, Piece to) {
    int matrice[4][4];
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            matrice[i][j]=from[j][3-i];
        }
    }

    copy_piece(matrice, to);
}

int compute_score (int level, int lines) {
    int scores[] = {0, 100, 400, 900, 2000};
    int lev=level;

    if (lev > 8)
        lev = 8;

    return (lev/2 + 1) * scores[lines];
}

int clear_lines (Board b) {
    int lines=0;
    for (int i=0 ; i<20; i++)
    {
        // check if a row is full
        bool full=true;
        for (int xx = 0 ; xx < 10 && full ; xx++)
        {
            if ( b[i][xx] == 0 )
                full=false;
        }

        //clear line
        if ( full )
        {
            for (int yy=i ; yy > 0 ; yy--)
            {
                for (int xx=0 ; xx < 10 ; xx++)
                {
                    b[yy][xx] = b[yy-1][xx];
                }
            }
            lines++;
        }
    }
    return lines;
}

void random_piece(Piece &pc) {
    int q = rand() % 7;
    copy_piece(pieces[q], pc);
}

void init_crt_piece (Player &p) {
    p.x = 3;
    p.y = 0;
    p.falling = false;
}

void next_piece (Player &p) {
    // freeze piece in last position on the board
    freeze_piece(p.x, p.y, p.piece, p.b);

    // clear lines
    int lines = clear_lines(p.b);
    p.cleared_lines += lines;

    p.score += compute_score(p.level, lines);
    if (p.cleared_lines >= 5)
    {
        p.level++;
        p.cleared_lines = 0;
    }

    init_crt_piece(p);
    copy_piece(p.next[0], p.piece);
    copy_piece(p.next[1], p.next[0]);
    copy_piece(p.next[2], p.next[1]);
    random_piece(p.next[2]);
}

int drop_speed (Player p) {
    if (p.falling) return 1;
    else return 50.0/(p.level*p.level+4)+1;
}

void p_rotate (Player &p) {
    int rot[4][4];
    rotate_piece(p.piece, rot);
    if (piece_fits(p.x, p.y, rot, p.b))
        copy_piece(rot, p.piece);
}

void p_right (Player &p) {
    if (piece_fits(p.x+1, p.y, p.piece, p.b)) p.x++;
}

void p_left (Player &p) {
    if (piece_fits(p.x-1, p.y, p.piece, p.b)) p.x--;
}

void p_fall (Player &p) {
    p.falling = true;
}

void init_player (Player &p) {
    for (int i=0; i<20; i++){
        for (int j=0; j<10; j++)
            p.b[i][j]=0;
    }
}

void process_key (SDL_Event event, SDL_Keycode up, SDL_Keycode rg, SDL_Keycode lf, SDL_Keycode dw, Player &p) {
    if (event.type == SDL_KEYDOWN) {
        const SDL_Keycode k = event.key.keysym.sym;
        if (k == up) p_rotate(p);
        if (k == rg) p_right(p);
        if (k == lf) p_left(p);
        if (k == dw) p_fall(p);
    }
}

int evaluate_board (Board b)
{
    int y=0, ymin=20, ymax=0, g=0, delta=0, yy=0, depth=0;
    for (int i=0; i<10; i++)
    {
        for (int j=0; j<20 && b[j][i]==0; j++)
            y = j;

        for (int j=y; j<20; j++) {
            if (b[j][i]==0) {
                g++;
                depth += j-y;
            }
        }

        if(i>0)
            delta += abs(y-yy)*abs(y-yy);

        if (y>ymax)
            ymax=y;

        if (y<ymin)
            ymin=y;

        yy=y;
    }
    return g*g + delta + (ymax-ymin)*(ymax-ymin) + depth;
}

void ai (Player &p)
{
    int evmax=1000, xmax=0, rmax=0;
    Piece pc1, pc2;
    copy_piece(p.piece, pc1);
    copy_piece(p.next[0], pc2);
    for (int i=-1; i<10; i++) {
        for (int j=0; j<4; j++) {
            if (piece_fits(i, p.y, pc1, p.b))
            {
                Board b;
                memcpy(b, p.b, sizeof(b));
                int y = p.y;
                while (piece_fits(i, y+1, pc1, b))
                    y++;
                freeze_piece(i, y, pc1, b);
                clear_lines(b);

                for (int k=-1; k<10; k++) {
                    for (int l=0; l<4; l++) {
                        if (piece_fits(k, 0, pc2, b))
                        {
                            Board bb;
                            memcpy(bb, b, sizeof(b));
                            int yy = 0;
                            while (piece_fits(k, yy+1, pc2, bb))
                                yy++;
                            freeze_piece(k, yy, pc2, bb);
                            clear_lines(bb);

                            int e = evaluate_board(bb);
                            if (e < evmax)
                            {
                                evmax = e;
                                xmax = i;
                                rmax = j;
                            }
                        }
                        rotate_piece(pc2, pc2);
                    }
                }
            }

            rotate_piece(pc1, pc1);
        }
    }
    if (rmax != 0)
        p_rotate(p);
    else if (xmax < p.x)
        p_left(p);
    else if (xmax > p.x)
        p_right(p);
    else p_fall(p);
}

int main (int argc, char* args[] ) {

    srand(time(NULL));
    rand();

    // empty boards
    for (int k=0; k<PLAYERS; k++)
        init_player(p[k]);

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if(TTF_Init()==-1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }
    Sans = TTF_OpenFont("Andale.ttf", 36);
    Small = TTF_OpenFont("Andale.ttf", 14);

    window = SDL_CreateWindow("", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    p[0].type = AI;
    p[1].type = P1;
    p[2].type = AI;

    // pick random piece and column
    for (int k=0; k<PLAYERS; k++) {
        random_piece(p[k].piece);
        random_piece(p[k].next[0]);
        random_piece(p[k].next[1]);
        random_piece(p[k].next[2]);
        init_crt_piece(p[k]);
    }

    int currentTime=0;

    // main game loop
    while (true)
    {
        // read human input
        SDL_Event event;
        bool keypressed = false;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) keypressed = true;
            if (event.type == SDL_QUIT) break;
        }

        currentTime = SDL_GetTicks();
        for (int i=0; i<PLAYERS; i++)
        {
            switch(p[i].type)
            {
                case P1: if (keypressed && !p[i].dead) process_key(event, SDLK_w, SDLK_d, SDLK_a, SDLK_s, p[i]); break;
                case P2: if (keypressed && !p[i].dead) process_key(event, SDLK_UP, SDLK_RIGHT, SDLK_LEFT, SDLK_DOWN, p[i]); break;
                case AI: if (currentTime > p[i].lastTime && !p[i].dead) {ai(p[i]); ai(p[i]); p[i].lastTime = currentTime;} break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (int k=0; k<PLAYERS; k++)
            print_board(p[k], 4 + k*18, 5);

        SDL_RenderPresent(renderer);

        for (int k=0; k<PLAYERS; k++)
        {
            if (!p[k].dead) {
                if (!piece_fits(p[k].x, p[k].y+1, p[k].piece, p[k].b)) {
                    // game over when new piece can't fit
                    if (p[k].y <= 0) {
                        //init_player(p[k]);
                        p[k].dead = true;
                    }
                    else next_piece(p[k]);
                }
                else {
                    p[k].drop_count++;
                    if (p[k].drop_count >= drop_speed(p[k])) {
                        p[k].y++;
                        p[k].drop_count = 0;
                    }
                }
            }
        }

//        SDL_Delay(16);
    }

    SDL_DestroyWindow( window );

    SDL_Quit();

}
