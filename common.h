//
//  common.h
//  tetris
//
//  Created by Mihnea Buzoiu on 6/27/22.
//

#ifndef common_h
#define common_h

#include <SDL.h>

typedef int Piece[4][4];
typedef int Board[20][10];

enum PlayerType {P1, P2, AI};

#define MAX_PLAYERS 10
extern int PLAYERS;

struct Player
{
    // player info
    PlayerType type;
    int difficulty=0;

    // state
    bool falling=false;
    bool dead = false;

    // scoring
    int level=0;
    int score=0;
    int cleared_lines=0;
    int level_clearing_lines=0;
    int focus_clearing_lines=0;
    int focus_cleared_lines=0;
    int drop_count=0;

    bool focus_active = false;
    int focus_timestamp = 0;

    // board state
    Board b;
    int x, y;
    Piece piece;
    Piece next[3];

    // ai
    bool new_piece = false;
    int ai_rotation=0;
    int ai_position=0;
    int lastTime=0;

    struct Textures {
        SDL_Texture *score;
        SDL_Texture *level;
        SDL_Texture *cleared_lines;
        SDL_Texture *focus_active;
        SDL_Texture *next_label;
        SDL_Texture *level_label;
        SDL_Texture *ability_label;
        SDL_Texture *player_type_label;
    } txt;
};

#endif /* common_h */
