const int SCREEN_WIDTH=1160;
const int SCREEN_HEIGHT=640;
const int w = 20;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

TTF_Font* Sans = NULL;
TTF_Font* Small = NULL;

int max(const int a, const int b) {
    return a<b? b : a;
}

void init_graphics() {
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }
    IMG_Init(IMG_INIT_JPG);

    if(TTF_Init()==-1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }

    Sans = TTF_OpenFont("Andale.ttf", 36);
    Small = TTF_OpenFont("Andale.ttf", 14);

    window = SDL_CreateWindow("", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

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
                SDL_SetRenderDrawColor(renderer, piece_colors[c][0], piece_colors[c][1], piece_colors[c][2], SDL_ALPHA_OPAQUE);

            }
            else {
                // stationary block
                int c = p.b[i][j];
                SDL_SetRenderDrawColor(renderer, piece_colors[c][0], piece_colors[c][1], piece_colors[c][2], SDL_ALPHA_OPAQUE);
            }
            SDL_RenderFillRect(renderer, &r);
        }
    }

    // draw next pieces
    int dx = (xoffset+1) * w;
    int dy = (yoffset+13) * w;
    for (int k=0; k < 3 - p.difficulty; k++) {
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                SDL_Rect r = { dx + j*(w/2), dy + ((k*4)+i)*(w/2) + k*w, w/2, w/2};
                int c = p.next[k][i][j];
                SDL_SetRenderDrawColor(renderer, piece_colors[c][0], piece_colors[c][1], piece_colors[c][2], SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }

    SDL_SetRenderDrawColor (renderer, 200, 0, 100, SDL_ALPHA_OPAQUE);
    int width=12;
    for (int k = max(3 - p.difficulty, 0); k<3; k++) {
        for (int i=0; i<width; i++) {
            SDL_RenderDrawLine (renderer, dx, dy + k*3*w + i, dx + 2*w, dy + (k*3+2)*w + i - width);
            SDL_RenderDrawLine (renderer, dx, dy + (k*3+2)*w + i - width, dx + 2*w, dy + k*3*w +i);
        }
    }
}

int selected_menu = 0;
void draw_titlescreen() {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // draw cover image
    SDL_Surface * image = IMG_Load("cover.jpg");
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);

    SDL_Rect dstrect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);

    // draw menu options
    SDL_Rect rectang1 = {200, 500, 100, 50};
    SDL_Rect rectang2 = {600, 500, 100, 50};
    SDL_Rect rectang3 = {1000, 500, 100, 50};

    if (selected_menu == 0) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &rectang1);
    draw_text("Solo", rectang1.x, rectang1.y, Sans, 255, 255, 255);

    if (selected_menu == 1) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &rectang2);
    draw_text("Coop", rectang2.x, rectang2.y, Sans, 255, 255, 255);

    if (selected_menu == 2) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &rectang3);
    draw_text("Quit", rectang3.x, rectang3.y, Sans, 255, 255, 255);

    SDL_RenderPresent(renderer);
}