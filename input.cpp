void process_key (SDL_Event event, SDL_Keycode up, SDL_Keycode rg, SDL_Keycode lf, SDL_Keycode dw, Player &p) {
    if (event.type == SDL_KEYDOWN) {
        const SDL_Keycode k = event.key.keysym.sym;
        if (k == up) p_rotate(p);
        if (k == rg) p_right(p);
        if (k == lf) p_left(p);
        if (k == dw) p_fall(p);
    }
}