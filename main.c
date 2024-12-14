#include <stdio.h>
#include <SDL2/SDL.h>
#include "utils.h"

#define WIDTH 800
#define HEIGHT 600
#define CELL_WIDTH 10
#define ROWS 60
#define COLUMNS 80

int WinMain(int argc, char* argv[])
{
    printf("Hello Conway\'s Game of Life\n");
    SDL_Init(SDL_INIT_VIDEO);
    char* window_title = "Conway\'s Game of Life";
    SDL_Window* window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    draw_grid(CELL_WIDTH, COLUMNS, ROWS, WIDTH, HEIGHT, surface);
    game_loop(WIDTH, HEIGHT, CELL_WIDTH, COLUMNS, ROWS, surface, window);
    return 0;
}
