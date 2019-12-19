#include <iostream>
//#include <ncurses.h>
#include <SDL.h>
#include <unistd.h>
#include <math.h>

using namespace std;

struct Player
{
    int level=1;
    int score;
    int cleared_lines;
    bool falling;
    int b[20][10];
    int x, y;
    int piece[4][4];

    int drop_count;
};

const int SCREEN_WIDTH=640;
const int SCREEN_HEIGHT=480;

int pieces[7][4][4] = {{{0, 0, 0, 0},
                       {0, 1, 1, 0},
                       {0, 1, 1, 0},
                       {0, 0, 0, 0}},

                      {{0, 0, 0, 0},
                       {2, 2, 2, 2},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0}},

                      {{0, 0, 0, 0},
                       {3, 3, 3, 0},
                       {0, 0, 3, 0},
                       {0, 0, 0, 0}},

                      {{0, 0, 4, 0},
                       {0, 4, 4, 0},
                       {0, 4, 0, 0},
                       {0, 0, 0, 0}},

                      {{0, 5, 0, 0},
                       {0, 5, 5, 0},
                       {0, 5, 0, 0},
                       {0, 0, 0, 0}},

                      {{0, 6, 6, 0},
                       {0, 0, 6, 0},
                       {0, 0, 6, 0},
                       {0, 0, 0, 0}},

                      {{0, 7, 0, 0},
                       {0, 7, 7, 0},
                       {0, 0, 7, 0},
                       {0, 0, 0, 0}}};

int colors[7][3]={{0xAA, 0xAA, 0xAA},
                  {0xAA, 0xAA, 0xAA},
                  {0xAA, 0xAA, 0xAA},
                  {0xAA, 0xAA, 0xAA},
                  {0xAA, 0xAA, 0xAA},
                  {0xAA, 0xAA, 0xAA},
                  {0xAA, 0xAA, 0xAA}};

const int w=15;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* screenSurface = NULL;

void print_board (Player p, int offset) {

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 13*w+offset*w, 0, 13*w+offset*w, 20*w);
    SDL_RenderDrawLine(renderer, 13*w+offset*w, 20*w, 23*w+offset*w, 20*w);
    SDL_RenderDrawLine(renderer, 23*w+offset*w, 20*w, 23*w+offset*w, 0);

    for (int i=0; i<20; i++)
    {
        for (int j=0; j<10; j++) {
            if ( i >= p.y && i < p.y+4 &&
                 j >= p.x && j < p.x+4 &&
                 p.piece[i-p.y][j-p.x] != 0)
            {
                // current piece
                int c = p.piece[i-p.y][j-p.x];
                SDL_SetRenderDrawColor(renderer, colors[c][0], colors[c][1], colors[c][2], SDL_ALPHA_OPAQUE);
                SDL_Rect r = {(13+j)*w+offset*w, i*w, w, w};
                SDL_RenderFillRect(renderer, &r);
            }
            else if (p.b[i][j]==0)
            {
                // blank space
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_Rect r = {(13+j)*w+offset*w, i*w, w, w};
                SDL_RenderFillRect(renderer, &r);
            }

            else {
                // stationary block
                int c = p.b[i][j];
                SDL_SetRenderDrawColor(renderer, colors[c][0], colors[c][1], colors[c][2], SDL_ALPHA_OPAQUE);
                SDL_Rect r = {(13+j)*w+offset*w, i*w, w, w};
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }

    SDL_RenderPresent(renderer);
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

int clear_lines(Player &p) {
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

void random_piece(Player &p) {
    int q = rand() % 7;
    copy_piece(pieces[q], p.piece);
    p.x = 5;
    p.y = 0;
    p.falling = false;
}

void next_piece(Player &p) {
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

int drop_speed(Player p) {
    if (p.falling) return 1;
    else return 30.0/sqrt(p.level+1);
}


#define PLAYERS 2
Player p[PLAYERS];

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

    if( SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer ))
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return -1;
    }

    // pick random piece and column
    for (int k=0; k<PLAYERS; k++)
        random_piece(p[k]);

    // main game loop
    while (true)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
        }

/*        int c = getch();
        switch (c)
        {
            case 'w':       {int rot[4][4]; rotate_piece(p[0].piece, rot);
                            if (piece_fits(p[0].x, p[0].y, rot, p[0].b)) copy_piece(rot, p[0].piece); break;}

            case KEY_UP:    {int rot[4][4]; rotate_piece(p[1].piece, rot);
                            if (piece_fits(p[1].x, p[1].y, rot, p[1].b)) copy_piece(rot, p[1].piece); break;}

            case KEY_RIGHT: if (piece_fits(p[1].x+1, p[1].y, p[1].piece, p[1].b)) p[1].x++; break;
            case 'd':       if (piece_fits(p[0].x+1, p[0].y, p[0].piece, p[0].b)) p[0].x++; break;

            case KEY_LEFT:  if (piece_fits(p[1].x-1, p[1].y, p[1].piece, p[1].b)) p[1].x--; break;
            case 'a':       if (piece_fits(p[0].x-1, p[0].y, p[0].piece, p[0].b)) p[0].x--; break;

            case KEY_DOWN:  p[1].falling = true; break;
            case 's':
            case ' ':       p[0].falling = true; break;
        }*/

//        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
//        SDL_RenderClear(renderer);

        for (int k=0; k<PLAYERS; k++)
            print_board(p[k], 30 * k);

        for (int k=0; k<PLAYERS; k++)
        {
            if (!piece_fits(p[k].x, p[k].y+1, p[k].piece, p[k].b)) {
                // game over when new piece can not fit
                if (p[k].y == 0) return -2;

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
