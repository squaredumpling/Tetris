const int SCREEN_WIDTH=1160;
const int SCREEN_HEIGHT=640;
const int w = 20;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

TTF_Font* Sans = NULL;
TTF_Font* Small = NULL;
TTF_Font* Bold = NULL;

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
    Bold = TTF_OpenFont("Andale.ttf", 36);
    TTF_SetFontStyle( Bold, TTF_STYLE_BOLD);

    window = SDL_CreateWindow("", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void create_text(SDL_Texture *&t, const char text[], TTF_Font* font, unsigned char r, unsigned char g, unsigned char b) {

    SDL_Color color_t = {r, g, b};
    SDL_Surface *surfaceMessage = TTF_RenderText_Blended(font, text, color_t);
    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

//    SDL_FreeSurface(surfaceMessage);
    if (t != NULL) SDL_DestroyTexture(t);
    t = Message;
}

void render_text(int x, int y, SDL_Texture *txt)
{
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(txt, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { x, y, texW, texH };

    SDL_RenderCopy(renderer, txt, NULL, &dstrect);
}

void init_player_textures(Player &p) {
    char txt[10]; // text holder
    int red = 0, green = 0, blue = 0;
    switch (p.type) {
        case P1:
            sprintf(txt, "P1");
            red = 200;
            green = 60;
            blue = 200;
            break;
        case P2:
            sprintf(txt, "P2");
            red = 200;
            green = 60;
            blue = 200;
            break;
        case AI:
            sprintf(txt, "AI");
            red = 60;
            green = 200;
            blue = 60;
            break;
    }

    create_text(p.txt.player_type_label, txt, Sans, red, green, blue);
    create_text(p.txt.level_label, "LEVEL", Small, 255, 255, 255);
    create_text(p.txt.ability_label, "ABILITY", Small, 255, 255, 255);
    create_text(p.txt.next_label, "NEXT", Small, 255, 255, 255);
}

int draw_rectangle(int x1, int y1, int x2, int y2){
    SDL_Rect rectangle = {x1, y1, x2-x1, y2-y1};
    return SDL_RenderFillRect(renderer, &rectangle);
}

void draw_gradient_line(int x, int y, int l) {;
    for (int i=0; i<l; i++) {
        int brightness = 250 * (l - i) / l;
        SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawPoint(renderer, x, y+i);
    }
}

void draw_indicators(Player p, int xoffset, int yoffset) {
    int x1=0, y1=0, x2=0, y2=0;
    bool left=true, right=true, middle=true;
    for (int i=0; i<3 && left; i++) {
        for (int j=0; j<4 && left; j++) {
            if (p.piece[j][i] != 0) {
                x1 = i;
                for (int k=3; k>=0; k--) {
                    if (p.piece[k][i]) {
                        y1 = k+1;
                        break;
                    }
                }
                left = false;
            }
        }
    }
    for (int i=3; i>0 && right; i--) {
        for (int j=0; j<4 && right; j++) {
            if (p.piece[j][i] != 0) {
                x2 = i+1;
                for (int k=3; k>=0; k--) {
                    if (p.piece[k][i]) {
                        y2 = k+1;
                        break;
                    }
                }
                right = false;
            }
        }
    }
    const int gap = 0;
    int l1 = 20 - gap - p.y - y1, l2 = 20 - gap - p.y - y2;;
    for (int i=0; i<20 && middle; i++) {
        for (int j = p.x+x1; j<p.x+x2 && middle; j++) {
            if (p.b[i][j] != 0) {
                l1 = max(i-p.y-y1-2, 0);
                l2 = max(i-p.y-y2-2, 0);
                middle = false;
            }
        }
    }
    draw_gradient_line((xoffset + p.x + x1) * w, (yoffset + p.y + y1) * w, l1*w);
    draw_gradient_line((xoffset + p.x + x2) * w - 1, (yoffset + p.y + y2) * w, l2*w);
}

void print_board (Player &p, int xoffset, int yoffset) {

    SDL_SetRenderDrawColor (renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    const int weight = 4;
    // border vertical
    draw_rectangle(xoffset*w-1-weight, yoffset*w-1, xoffset*w-1, (22+yoffset)*w);
    draw_rectangle((xoffset+4)*w-1-weight, yoffset*w-1, (xoffset+4)*w-1, (22+yoffset)*w);
    draw_rectangle((xoffset+14)*w, yoffset*w-1, (xoffset+14)*w+1+weight, (22+yoffset)*w);
    // border horizontal
    draw_rectangle(xoffset*w-1-weight, yoffset*w-1-weight, (xoffset+14)*w+1+weight, yoffset*w-1);
    draw_rectangle(xoffset*w-1-weight, (yoffset+2)*w-1-weight, (xoffset+14)*w+1+weight, (yoffset+2)*w-1);
    draw_rectangle(xoffset*w-1-weight, (yoffset+22)*w, (xoffset+14)*w+1+weight, (yoffset+22)*w+1+weight);
    //small square 1
    SDL_Rect s1 = {xoffset*w-1+w/2 - weight/2, (yoffset+2)*w-1+w/2, w*3+2, w*3+2+w/2};
    SDL_RenderDrawRect(renderer, &s1);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2 - weight/2, (yoffset+3)*w-1+w/2, (xoffset+3)*w-1+w/2 - weight/2, (yoffset+3)*w-1+w/2);
    // small square 2
    SDL_Rect s2 = {xoffset*w-1+w/2 - weight/2, (yoffset+7)*w-1, w*3+2, w*3+2+w/2};
    SDL_RenderDrawRect(renderer, &s2);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2 - weight/2, (yoffset+8)*w-1, (xoffset+3)*w-1+w/2 - weight/2, (yoffset+8)*w-1);
    // next piece rectangle
    SDL_Rect s3 = {xoffset*w-1+w/2 - weight/2, (yoffset+11)*w-1+w/2, w*3+2, 10*w};
    SDL_RenderDrawRect(renderer, &s3);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2 - weight/2, (yoffset+12)*w + w/2, (xoffset+3)*w-1 + w/2 - weight/2, (yoffset+12)*w + w/2);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2 - weight/2, (yoffset+15)*w + w/2, (xoffset+3)*w-1 + w/2 - weight/2, (yoffset+15)*w + w/2);
    SDL_RenderDrawLine(renderer, xoffset*w-1+w/2 - weight/2, (yoffset+18)*w + w/2, (xoffset+3)*w-1 + w/2 - weight/2, (yoffset+18)*w + w/2);

    char txt[10]; // text holder

    if (can_activate_focus(p)){
        sprintf(txt, "F");
        create_text(p.txt.focus_active, txt, Sans, 255, 255, 255);
    }

    if (p.focus_active){
        sprintf(txt, "X");
        create_text(p.txt.focus_active, txt, Sans, 255, 255, 255);
    }

    if (p.new_piece) {
        sprintf(txt, "%7d", p.cleared_lines);
        create_text(p.txt.cleared_lines, txt, Sans, 255, 255, 255);

        if (p.level < 10) {
            sprintf(txt, "%d", p.level);
            create_text(p.txt.level, txt, Bold, 255, 255, 255);
        }
        else {
            sprintf(txt, "X");
            create_text(p.txt.level, txt, Bold, 128, 0, 255);
        }
    }

    render_text((xoffset + 6) * w, yoffset * w - weight/2, p.txt.cleared_lines);
    render_text((xoffset + 1) * w + w/2 - weight/2, (yoffset + 4) * w - w / 4, p.txt.level);
    render_text(xoffset * w + w / 2 + 2 - weight/2, (yoffset + 8) * w, p.txt.focus_active);
    render_text((xoffset + 1) * w - weight/2, yoffset * w - weight, p.txt.player_type_label);
    render_text((xoffset + 1) * w - weight/2, (yoffset + 2) * w + w / 2 + 2, p.txt.level_label);
    render_text(xoffset * w + w / 2 + 2 - weight/2, (yoffset + 7) * w + 2, p.txt.ability_label);
    render_text(xoffset * w + w + 4 - weight/2, (yoffset + 11) * w + w / 2 + 2, p.txt.next_label);

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

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    draw_indicators(p, xoffset+4, yoffset+2);

    // draw next pieces
    int dx = (xoffset+1) * w - weight/2;
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
    SDL_Rect rectang0 = {315, 470, 270, 50};
    SDL_Rect rectang1 = {640, 470, 270, 50};
    SDL_Rect rectang2 = {550, 550, 120, 50};
    SDL_Rect rectang3 = {1080, 30, 50, 50};

    if (selected_menu == 0) {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &rectang0);
    SDL_Texture *s = NULL;
    create_text(s, "Singleplayer", Sans, 255, 255, 255);
    render_text(rectang0.x + 4, rectang0.y + 4, s);

    if (selected_menu == 1) {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &rectang1);
    create_text(s, "Multiplayer", Sans, 255, 255, 255);
    render_text(rectang1.x + 15, rectang1.y + 4, s);

    if (selected_menu == 2) {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &rectang2);
    create_text(s, "Quit", Sans, 255, 255, 255);
    render_text(rectang2.x + 15, rectang2.y + 4, s);

    if (selected_menu == 50) {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRect(renderer, &rectang3);
    create_text(s, "ƒ", Sans, 255, 255, 255);
    render_text(rectang3.x, rectang3.y + 4, s);

    SDL_RenderPresent(renderer);
}