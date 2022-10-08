#include <math.h>
#include "common.h"
#include "mechanics.h"

int evaluate_board (Board b)
{
    int y=0, ymin=20, ymax=0, hole=0, delta=0, yy=0, depth=0;
    for (int i=0; i<10; i++)
    {
        for (int j=0; j<20 && b[j][i]==0; j++)
            y = j;

        for (int j=y; j<20; j++) {
            if (b[j][i]==0) {
                hole++;
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
    return hole*hole + delta + (ymax-ymin)*(ymax-ymin) + depth;
}

void compute_next_piece (Player &p)
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
    p.ai_rotation = rmax;
    p.ai_position = xmax;
}

void ai_move (Player &p)
{
    if (p.new_piece) {
        compute_next_piece(p);
        p.new_piece = false;
    }

    if (p.ai_rotation != 0) {
        p_rotate(p);
        p.ai_rotation--;
    }
    else if (p.ai_position < p.x)
        p_left(p);
    else if (p.ai_position > p.x)
        p_right(p);
    else p_fall(p);
}
