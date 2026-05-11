#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

static constexpr int WIDTH = 1280;
static constexpr int HEIGHT = 720;

struct app_info {
    SDL_Window *window;
    SDL_GLContext ctx;
};

bool init_sdl(struct app_info *app, int w, int h);

int main(void) {

    struct app_info app = {};

    if (!init_sdl(&app, WIDTH, HEIGHT)) {
        return EXIT_FAILURE;
    }
    SDL_Event e;
    bool active = true;
    while (active) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT:
                active = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (e.key.key) {
                case SDLK_ESCAPE:
                    active = false;
                    break;
                }
                break;
            }
        }

        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(0.1f, 0.0f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(app.window);
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}

bool init_sdl(struct app_info *app, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Eror initalizing SDL: %s\n", SDL_GetError());
        return false;
    }
    SDL_Window *window;

    window = SDL_CreateWindow("Hello, World", width, height, SDL_WINDOW_OPENGL);
    if (!window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, ctx);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Failed to load GL");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    app->window = window;
    app->ctx = ctx;
    const char *version = glGetString(GL_VERSION);
    if (version != nullptr) {
        printf("OpenGL Version: %s\n", version);
    }
    return true;
}
