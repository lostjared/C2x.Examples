#ifndef __CMX_GL__
#define __CMX_GL__

#include<SDL3/SDL.h>
#include<glad/glad.h>

struct app_info {
    SDL_Window *window;
    SDL_GLContext ctx;
    int w, h;
};

bool init_sdl(struct app_info *app, int w, int h);
int close_sdl(struct app_info *app);
GLuint load_spv(GLenum type, const char *path);
char *read_file(const char *path, size_t *sz); 

#endif
