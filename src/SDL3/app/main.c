#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

static constexpr Uint32 WINDOW_WIDTH = 1280;
static constexpr Uint32 WINDOW_HEIGHT = 720;

int main(int argc, char **argv) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Error init of SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    printf("sdl3: SDL_Init\n");
    SDL_Window *window = SDL_CreateWindow("Hello, World", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    printf("sdl3: SDL_CreateWindow\n");
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    printf("sdl3: CreateRenderer\n");
    SDL_Event e;
    bool active = true;
    while (active) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT:
                active = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (e.key.key == SDLK_ESCAPE)
                    active = false;
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    printf("sdl3: SDL_DestroyRenderer\n");
    SDL_DestroyWindow(window);
    printf("sdl3: SDL_DestroyWindow\n");
    SDL_Quit();
    printf("sdl3: SDL_Quit\n");
    return EXIT_SUCCESS;
}
