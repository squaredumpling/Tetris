#include <iostream>
#include <ncurses.h>
#include <unistd.h>
using namespace std;

int pieces[7][4][4] = {{{1, 1, 0, 0},
                       {1, 1, 0, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0}},

                      {{2, 2, 2, 2},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0}},

                      {{3, 3, 3, 0},
                       {0, 0, 3, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0}},

                      {{0, 4, 0, 0},
                       {4, 4, 0, 0},
                       {4, 0, 0, 0},
                       {0, 0, 0, 0}},

                      {{0, 5, 0, 0},
                       {5, 5, 5, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0}},

                      {{0, 0, 6, 0},
                       {6, 6, 6, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0}},

                      {{7, 0, 0, 0},
                       {7, 7, 0, 0},
                       {0, 7, 0, 0},
                       {0, 0, 0, 0}}};

int width[7] = {2, 4, 3, 2, 3, 3, 2};

void print_board(int x, int y, int p, int b[20][10]) {
    for (int i=0; i<20; i++)
    {
        cout << "|";
        for (int j=0; j<10; j++) {
            if ( i >= y && i < y+4 &&
                 j >= x && j < x+4 &&
                 pieces[p][i-y][j-x] != 0)
                cout << "[]";
            else if (b[i][j]==0)
                cout << "  ";
            else cout <<"{}";

        }
        cout << "|" << endl;
    }
    for (int i=0; i<22; i++)
        cout << "–";
    cout << endl << endl;
}

int random_column (int p)
{
    return rand() % (11 - width[p]);
}

void freeze_piece (int x, int y, int p, int b[20][10])
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (pieces[p][i][j] != 0)
                b[y+i][x+j] = p+1;
        }
    }
}

bool piece_fits (int x, int y, int p, int b[20][10])
{
    bool yes = true;
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            if (pieces[p][i][j] != 0 && b[y+i][x+j] != 0)
                yes = false;
        }
    }
    return yes;
}



int main() {

    int x, y, b[20][10];

    srand(time(NULL));
    
    // empty board
    for (int i=0; i<20; i++){
        for (int j=0; j<10; j++)
            b[i][j]=0;
    }

    // main game loop
    for (int k=0; k<150; k++)
    {
        // pick random piece and column
        int p = rand() % 7;
        x = random_column(p);

        // fall piece loop
        for (y = 0; y < 20 && piece_fits(x, y, p, b) ; y++)
        {
            print_board(x, y, p, b);
            usleep(500000);
        }

        // game over when new piece can not fit
        if (y==0)
            return -1;

        // freeze piece in last position on the board
        freeze_piece(x, y-1, p, b);

        // check if a row is full
        bool full=true;
        for (int xx = 0 ; xx < 10 && full ; xx++)
        {
            if ( b[y-1][xx] == 0 )
                full=false;
        }

        //clear line
        if ( full )
        {
            for (int yy=y-1 ; yy > 0 ; yy--)
            {
                for (int xx=0 ; xx < 10 ; xx++)
                {
                    b[yy][xx] = b[yy-1][xx];
                }
            }
            usleep(1000000);
            print_board(x, y, p, b);
            usleep(3000000);
        }
    }



}