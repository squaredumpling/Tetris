#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <math.h>

using namespace std;

struct Player
{
    int level;
    int score;
    int cleared_lines;
    int lines;
    int b[20][10];
    int x, y;
    int piece[4][4];
};

Player p1, p2;

//int level=1, score=0, cleared_lines=0;

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

void print_board (Player p, int offset) {
    for (int i=0; i<20; i++)
    {
        move(i+1, 20+offset);
        addch('|');
        for (int j=0; j<10; j++) {
            if ( i >= p.y && i < p.y+4 &&
                 j >= p.x && j < p.x+4 &&
                 p.piece[i-p.y][j-p.x] != 0)
                {
                    // current piece
                    attron(COLOR_PAIR(p.piece[i-p.y][j-p.x]));
                    printw("  ");
                    attroff(COLOR_PAIR(p.piece[i-p.y][j-p.x]));
                }
            else if (p.b[i][j]==0)
                // blank space
                printw("  ");
            else {
                // stationary block
                attron(COLOR_PAIR(p.b[i][j]));
                printw("  ");
                attroff(COLOR_PAIR(p.b[i][j]));
            }

        }
        addch('|');
    }
    move(21, 20+offset);
    for (int i=0; i<22; i++)
        addch('-');
    move(22,0+offset);

    mvprintw(1,0+offset, "Score: %d", p.score);
    mvprintw(2,0+offset, "Level: %d", p.level);

    refresh();
}

void freeze_piece (Player p)
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (p.piece[i][j] != 0)
                p.b[p.y+i][p.x+j] = p.piece[i][j];
        }
    }
}

bool piece_fits (int x, int y, int pc[4][4], int b[20][10])
{
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

void copy_piece (int from[4][4], int to[4][4])
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++){
            to[i][j]=from[i][j];
        }
    }
}

void rotate_piece (int from[4][4], int to[4][4])
{
       for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            to[i][j]=from[j][3-i];
        }
    }
}

int compute_score (int level, int lines)
{
    int scores[] = {0, 100, 400, 900, 2000};
    int lev=level;

    if (lev > 8)
        lev = 8;

    return (lev/2 + 1) * scores[lines];
}


int main() {

    //int b[20][10];

    srand(time(NULL));

    // empty board
    for (int i=0; i<20; i++){
        for (int j=0; j<10; j++)
            p1.b[i][j]=p2.b[i][j]=0;
    }

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, true);
    timeout(0);
    start_color();
    for (int i=0; i<7; i++)
        init_pair(i+1, i+1, i+1);

    // main game loop
    for (int k=0; k<150; k++)
    {
        // pick random piece and column
        //int piece[4][4];
        int q = rand() % 7;
        copy_piece(pieces[q], p1.piece);
        p1.x = 5;

        // fall piece loop
        //int y = 0;
        bool falling = false;
        while (piece_fits(p1.x, p1.y, p1.piece, p1.b))
        {
            for (int j=0; j<50; j++){
                int c = getch();
                switch (c)
                {
                    case KEY_UP:
                    case 'w':       int rot[4][4]; rotate_piece(p1.piece, rot);
                                    if (piece_fits(p1.x, p1.y, rot, p1.b)) copy_piece(rot, p1.piece); break;

                    case KEY_RIGHT:
                    case 'd':       if (piece_fits(p1.x+1, p1.y, p1.piece, p1.b)) p1.x++; break;

                    case KEY_LEFT:
                    case 'a':       if (piece_fits(p1.x-1, p1.y, p1.piece, p1.b)) p1.x--; break;

                    case KEY_DOWN:
                    case 's':
                    case ' ':       falling = true; break;
                }
                print_board(p1, 0);
                if (!falling)
                    usleep(10000.0/sqrt(p1.level+1));
            }
            p1.y++;
        }

        // game over when new piece can not fit
        if (p1.y==0)
            return -1;

        // freeze piece in last position on the board
        freeze_piece(p1);

        p1.lines=0;

        for (int i=0 ; i<4 && p1.y+i<21; i++)
        {
            // check if a row is full
            bool full=true;
            for (int xx = 0 ; xx < 10 && full ; xx++)
            {
                if ( p1.b[p1.y-1+i][xx] == 0 )
                    full=false;
            }

            //clear line
            if ( full )
            {
                for (int yy=p1.y-1+i ; yy > 0 ; yy--)
                {
                    for (int xx=0 ; xx < 10 ; xx++)
                    {
                        p1.b[yy][xx] = p1.b[yy-1][xx];
                    }
                }
                p1.lines++;
            }
        }
        p1.cleared_lines += p1.lines;
        if (p1.cleared_lines >= 5)
        {
            p1.level++;
            p1.cleared_lines = 0;
        }
        p1.score += compute_score(p1.level, p1.lines);
    }

    endwin();

}
