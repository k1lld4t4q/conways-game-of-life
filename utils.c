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
          field[j + columns * i] = 0;  
        }
    }
}

void edit_field(int x, int y, int columns, int field[])
{
    field[x + columns * y] = !field[x + columns * y];
}

void draw_field(int rows, int columns, int cell_width, SDL_Surface* surface, int field[])
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (field[j + columns * i]) {
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
        counter += field[(x - 1) + (y - 1) * columns];
    }

    if (y > 0) {
        counter += field[x + (y - 1) * columns];
    }

    if (x < columns - 1 && y > 0) {
        counter += field[(x + 1) + (y - 1) * columns];
    }
    
    if (x > 0) {
        counter += field[(x - 1) + y * columns];
    }

    if (x < columns - 1) {
        counter += field[(x + 1) + y * columns];
    }

    if (x > 0 && y < rows - 1) {
        counter += field[(x - 1) + (y + 1) * columns];
    }

    if (y < rows - 1) {
        counter += field[x + (y + 1) * columns];
    }

    if (x < columns - 1 && y < rows - 1) {
        counter += field[(x + 1) + (y + 1) * columns];
    }

    return counter;
}

void simulate(int rows, int columns, int field[])
{
    int new_field[rows * columns];
    init_field(rows, columns, new_field);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int cell_status = field[j + i * columns];
            int neighbours = count_neighbours(j, i, rows, columns, field);
            if (cell_status == 1 && neighbours < 2) {
                new_field[j + i * columns] = 0;
            }

            if (cell_status == 1 && (neighbours == 2 || neighbours == 3)) {
                new_field[j + i * columns] = 1;
            }

            if (cell_status == 1 && neighbours > 3) {
                new_field[j + i * columns] = 0;
            }

            if (cell_status == 0 && neighbours == 3) {
                new_field[j + i * columns] = 1;
            }
        }
    }

    for (int n = 0; n < rows * columns; n++) {
        field[n] = new_field[n];
    }
}

int update(int width, int height, int cell_width, int columns, int rows, SDL_Surface* surface, int field[], Coordinates* cords, Settings* settings)
{
    SDL_Rect clearscr = (SDL_Rect) {0, 0, width, height};

    SDL_PumpEvents();
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (settings->simulation) {
        simulate(rows, columns, field);
    }

    if (settings->cursor) {
        if (cords->x != cords->prevx || cords->y != cords->prevy) {
            clear_cursor(cords->prevx, cords->prevy, cell_width, surface);
        }
        draw_cursor(cords->x, cords->y, cell_width, surface);
    }

    cords->prevx = cords->x;
    cords->prevy = cords->y;

    if (keys[SDL_SCANCODE_Q]) {
        return 0;
    }

    if (keys[SDL_SCANCODE_T]) {
        settings->simulation = !settings->simulation;
    }

    if (keys[SDL_SCANCODE_K]) {
        clear_cursor(cords->x, cords->y, cell_width, surface);
        settings->cursor = !settings->cursor;
    }

    if (keys[SDL_SCANCODE_C]) {
        SDL_FillRect(surface, &clearscr, COLOR_BLACK);
        draw_grid(cell_width, columns, rows, width, height, surface);
        init_field(rows, columns, field);
    }

    if (keys[SDL_SCANCODE_SPACE] && settings->cursor) {
        edit_field(cords->x, cords->y, columns, field);
    }

    if (keys[SDL_SCANCODE_D] && settings->cursor) {
        cords->x += 1;
        if (cords->x > width / cell_width-1) {
            cords->x = 0;
        }
    }

    if (keys[SDL_SCANCODE_A] && settings->cursor) {
        cords->x -= 1;
        if (cords->x < 0) {
            cords->x = width / cell_width-1;
        }
    }

    if (keys[SDL_SCANCODE_S] && settings->cursor) {
        cords->y += 1;
        if (cords->y > height / cell_width-1) {
            cords->y = 0;
        }
	}

    if (keys[SDL_SCANCODE_W] && settings->cursor) {
        cords->y -= 1;
        if (cords->y < 0) {
            cords->y = height / cell_width-1;
        }
    }

    if (keys[SDL_SCANCODE_M]) {
        if (settings->update_ms > 100) {
            return 1;
        }
        settings->update_ms += 1;
    }

    if (keys[SDL_SCANCODE_P]) {
        if (settings->update_ms < 10) {
            return 1;
        }
        settings->update_ms -= 1;
    }

    draw_field(rows, columns, cell_width, surface, field);

    return 1;
}

void game_loop(int width, int height, int cell_width, int columns, int rows, SDL_Surface* surface, SDL_Window* window)
{
    int game = 1;
    int field[rows * columns];
    init_field(rows, columns, field);
    Settings settings = {1, 50, 0};
    Settings *pSettings = &settings;
    Coordinates cords = {0, 0, 0, 0};

    while (game) {
        game = update(width, height, cell_width, columns, rows, surface, field, &cords, &settings);
        SDL_UpdateWindowSurface(window);
        Sleep(pSettings->update_ms);
    }

    return;
}
