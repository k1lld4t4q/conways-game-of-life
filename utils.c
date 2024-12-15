#include <stdio.h>
#include <SDL2/SDL.h>
#include <windows.h>

#define COLOR_WHITE  0xffffffff
#define COLOR_BLACK  0x00000000
#define COLOR_CELL   0xcccccccc
#define COLOR_CURSOR 0x00dd0000

typedef struct {
    int x;
    int y;
    int prevx;
    int prevy;
} Coordinates;

typedef struct {
    int cursor;
    int update_ms;
    int simulation;
} Settings;

void draw_grid(int cell_width, int columns, int rows, int width, int height, SDL_Surface* surface)
{
    for (int i = 0; i < rows; i++) {
        SDL_Rect line = (SDL_Rect) {0, i*cell_width, width, 1};
        SDL_FillRect(surface, &line, COLOR_WHITE);
    }

    for (int j = 0; j < columns; j++) {
        SDL_Rect line = (SDL_Rect) {j*cell_width, 0, 1, height};
        SDL_FillRect(surface, &line, COLOR_WHITE);
    }
}

void draw_cell(int cell_x, int cell_y, int cell_width, SDL_Surface* surface, Uint32 color)
{
    SDL_Rect cell = (SDL_Rect) {cell_x*cell_width+1, cell_y*cell_width+1, cell_width-1, cell_width-1};
    SDL_FillRect(surface, &cell, color);
}

void clear_cell(int cell_x, int cell_y, int cell_width, SDL_Surface* surface)
{
    SDL_Rect cell = (SDL_Rect) {cell_x*cell_width+1, cell_y*cell_width+1, cell_width-1, cell_width-1};
    SDL_FillRect(surface, &cell, COLOR_BLACK);
}

void draw_cursor(int cell_x, int cell_y, int cell_width, SDL_Surface* surface)
{
    SDL_Rect up_side = (SDL_Rect) {cell_x*cell_width, cell_y*cell_width, cell_width, 1};
    SDL_Rect down_side = (SDL_Rect) {cell_x*cell_width, cell_y*cell_width+cell_width, cell_width, 1};
    SDL_Rect left_side = (SDL_Rect) {cell_x*cell_width, cell_y*cell_width, 1, cell_width};
    SDL_Rect right_side = (SDL_Rect) {cell_x*cell_width+cell_width, cell_y*cell_width, 1, cell_width};

    SDL_FillRect(surface, &up_side, COLOR_CURSOR);
    SDL_FillRect(surface, &down_side, COLOR_CURSOR);
    SDL_FillRect(surface, &left_side, COLOR_CURSOR);
    SDL_FillRect(surface, &right_side, COLOR_CURSOR);
}

void clear_cursor(int cell_x, int cell_y, int cell_width, SDL_Surface* surface)
{
    SDL_Rect up_side = (SDL_Rect) {cell_x*cell_width, cell_y*cell_width, cell_width, 1};
    SDL_Rect down_side = (SDL_Rect) {cell_x*cell_width, cell_y*cell_width+cell_width, cell_width, 1};
    SDL_Rect left_side = (SDL_Rect) {cell_x*cell_width, cell_y*cell_width, 1, cell_width};
    SDL_Rect right_side = (SDL_Rect) {cell_x*cell_width+cell_width, cell_y*cell_width, 1, cell_width};

    SDL_FillRect(surface, &up_side, COLOR_WHITE);
    SDL_FillRect(surface, &down_side, COLOR_WHITE);
    SDL_FillRect(surface, &left_side, COLOR_WHITE);
    SDL_FillRect(surface, &right_side, COLOR_WHITE);
}

void init_field(int rows, int columns, int field[])
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
          field[j+columns*i] = 0;  
        }
    }
}

void edit_field(int x, int y, int columns, int field[])
{
    field[x+columns*y] = !field[x+columns*y];
}

void draw_field(int rows, int columns, int cell_width, SDL_Surface* surface, int field[])
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (field[j+columns*i]) {
                draw_cell(j, i, cell_width, surface, COLOR_CELL);
            } else {
                draw_cell(j, i, cell_width, surface, COLOR_BLACK);
            }
        }
    }
}

int count_neighbours(int x, int y, int rows, int columns, int field[])
{
    int counter = 0;

    if (x > 0 && y > 0) {
        counter += field[(x-1)+(y-1)*columns];
    }

    if (y > 0) {
        counter += field[x+(y-1)*columns];
    }

    if (x < columns-1 && y > 0) {
        counter += field[(x+1)+(y-1)*columns];
    }
    
    if (x > 0) {
        counter += field[(x-1)+y*columns];
    }

    if (x < columns-1) {
        counter += field[(x+1)+y*columns];
    }

    if (x > 0 && y < rows-1) {
        counter += field[(x-1)+(y+1)*columns];
    }

    if (y < rows-1) {
        counter += field[x+(y+1)*columns];
    }

    if (x < columns-1 && y < rows-1) {
        counter += field[(x+1)+(y+1)*columns];
    }

    return counter;
}

void simulate(int rows, int columns, int field[])
{
    int new_field[rows*columns];
    init_field(rows, columns, new_field);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int cell_status = field[j+i*columns];
            int neighbours = count_neighbours(j, i, rows, columns, field);
            if (cell_status == 1 && neighbours < 2) {
                new_field[j+i*columns] = 0;
            }

            if (cell_status == 1 && (neighbours == 2 || neighbours == 3)) {
                new_field[j+i*columns] = 1;
            }

            if (cell_status == 1 && neighbours > 3) {
                new_field[j+i*columns] = 0;
            }

            if (cell_status == 0 && neighbours == 3) {
                new_field[j+i*columns] = 1;
            }
        }
    }

    for (int n = 0; n < rows*columns; n++) {
        field[n] = new_field[n];
    }
}

int update(int cell_width, int columns, int rows, SDL_Surface* surface, int field[], Coordinates* pCords, Settings* pSettings)
{
    if (pSettings->simulation) {
        simulate(rows, columns, field);
    }

    if (pSettings->cursor) {
        clear_cursor(pCords->prevx, pCords->prevy, cell_width, surface);
        draw_cursor(pCords->x, pCords->y, cell_width, surface);
    }

    draw_field(rows, columns, cell_width, surface, field);

    return 1;
}

void game_loop(int width, int height, int cell_width, int columns, int rows, SDL_Surface* surface, SDL_Window* window)
{
    int game = 1;
    int field[rows*columns];
    init_field(rows, columns, field);
    Settings settings = {1, 50, 0};
    Settings *pSettings = &settings;
    Coordinates cords = {0, 0, 0, 0};
    Coordinates *pCords = &cords;
    SDL_Event event;
    SDL_Rect clearscr = (SDL_Rect){0, 0, columns, rows};

    while (game) {
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_q:
                            return;

                        case SDLK_t:
                            pSettings->simulation = !pSettings->simulation;
                            break;

                        case SDLK_c:
                            SDL_FillRect(surface, &clearscr, COLOR_BLACK);
                            draw_grid(cell_width, columns, rows, width, height, surface);
                            init_field(rows, columns, field);
                            break;

                        case SDLK_k:
                            clear_cursor(pCords->x, pCords->y, cell_width, surface);
                            pSettings->cursor = !pSettings->cursor;
                            break;

                        case SDLK_m: 
                            pSettings->update_ms += 10;
                            if (pSettings->update_ms > 100) { pSettings->update_ms = 100; }
                            break;

                        case SDLK_p:
                            pSettings->update_ms += -10;
                            if (pSettings->update_ms < 10) { pSettings->update_ms = 10; }
                            break;

                        case SDLK_SPACE:
                            if (pSettings->cursor) {
                                edit_field(pCords->x, pCords->y, columns, field);
                                break;
                            }

                        case SDLK_w:
                            if (pSettings->cursor) {
                                pCords->y += -1;
                                if (pCords->y < 0) { pCords->y = rows-1; }
                            }
                            break;

                        case SDLK_a:
                            if (pSettings->cursor) {
                                pCords->x += -1;
                                if (pCords->x < 0) { pCords->x = columns-1; }
                            }
                            break;
            
                        case SDLK_s:
                            if (pSettings->cursor) {
                                pCords->y += 1;
                                if (pCords->y > rows-1) { pCords->y = 0; }
                            }
                            break;

                        case SDLK_d:
                            if (pSettings->cursor) {
                                pCords->x += 1;
                                if (pCords->x > columns-1) { pCords->x = 0;  }
                            }
                            break;

                    }
                default:
                    break;
            }
        }

        game = update(cell_width, columns, rows, surface, field, pCords, pSettings);
        SDL_UpdateWindowSurface(window);

        pCords->prevx = pCords->x;
        pCords->prevy = pCords->y;

        Sleep(pSettings->update_ms);
    }

    return;
}
