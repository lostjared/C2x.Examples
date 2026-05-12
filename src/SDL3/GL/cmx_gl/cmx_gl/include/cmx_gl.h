#ifndef __CMX_GL__
#define __CMX_GL__

#include<SDL3/SDL.h>
#include<glad/glad.h>

struct mx_app_info {
    SDL_Window *window;
    SDL_GLContext ctx;
    int w, h;
};

bool mx_init_sdl(struct mx_app_info *app, int w, int h);
int mx_close_sdl(struct mx_app_info *app);
GLuint mx_load_spv(GLenum type, const char *path);
char *mx_read_file(const char *path, size_t *sz); 

#endif
