#ifndef __CMX_GL__
#define __CMX_GL__

#include<SDL3/SDL.h>
#include<glad/glad.h>

#define MX_CHECK_GL_ERROR() \
{ \
        GLenum err = glGetError(); \
        if (err != GL_NO_ERROR) \
            printf("OpenGL Error: %d at %s:%d\n", err, __FILE__, __LINE__); \
}

#ifdef __cplusplus
extern "C" {
#endif

struct mx_app_info {
    SDL_Window *window;
    SDL_GLContext ctx;
    int w, h;
};

bool mx_init_sdl(struct mx_app_info *app, const char *title, int w, int h);
int mx_close_sdl(struct mx_app_info *app);
char *mx_read_file(const char *path, size_t *sz); 
GLuint mx_load_spv(GLenum type, const char *path);
GLuint mx_create_shader_program(const char *vertex_spv, const char *fragment_spv);
GLuint mx_create_compute_program(const char *compute_spv);

#ifdef __cplusplus
}
#endif

#endif
