
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

void init_game() {
    // empty boards
    for (int k=0; k<PLAYERS; k++)
        init_player(p[k]);

    p[0].type = AI;
    p[1].type = P1;
    p[2].type = AI;

    p[0].difficulty = 0;
    p[1].difficulty = 2;
    p[2].difficulty = 1;

    // pick random piece and column
    for (int k=0; k<PLAYERS; k++) {
        random_piece(p[k].piece);
        random_piece(p[k].next[0]);
        random_piece(p[k].next[1]);
        random_piece(p[k].next[2]);
        init_crt_piece(p[k]);
    }
}

void next_frame(Player &p) {
    if (!p.dead) {
        if (!piece_fits(p.x, p.y+1, p.piece, p.b)) {
            // game over when new piece can't fit
            if (p.y <= 0) {
                //init_player(p[k]);
                p.dead = true;
            }
            else next_piece(p);
        }
        else {
            p.drop_count++;
            if (p.drop_count >= drop_speed(p)) {
                p.y++;
                p.drop_count = 0;
            }
        }
    }
}