#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

int draw_grid(int cell_width, int columns, int rows, int swidth, int sheight, SDL_Surface* surface);

void game_loop(int width, int height, int cell_width, int columns, int rows, SDL_Surface* surface, SDL_Window* window);

#endif
