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
