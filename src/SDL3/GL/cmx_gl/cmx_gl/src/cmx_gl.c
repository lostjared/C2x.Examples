#include <cmx_gl.h>
#include <stdio.h>
#include <stdlib.h>

bool mx_init_sdl(struct mx_app_info *app, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Eror initalizing SDL: %s\n", SDL_GetError());
        return false;
    }
    SDL_Window *window;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("Hello, World", width, height, SDL_WINDOW_OPENGL);
    if (!window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
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
    app->w = width;
    app->h = height;

    const char *version = (const char *)glGetString(GL_VERSION);
    if (version != nullptr) {
        printf("OpenGL Version: %s\n", version);
    }
    return true;
}

int mx_close_sdl(struct mx_app_info *app) {
    if (app->ctx) {
        if (!SDL_GL_DestroyContext(app->ctx)) {
            fprintf(stderr, "Error destroying GL Context: %s\n", SDL_GetError());
            if (app->window)
                SDL_DestroyWindow(app->window);
            SDL_Quit();
            return EXIT_FAILURE;
        }
    }
    if (app->window != nullptr) {
        SDL_DestroyWindow(app->window);
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}

char *mx_read_file(const char *path, size_t *sz) {
    if (path == nullptr || sz == nullptr) {
        fprintf(stderr, "Invalid Input for read_file.\n");
        return nullptr;
    }
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Error loading file: %s\n", path);
        return nullptr;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    if (size == 0) {
        fprintf(stderr, "Empty file: %s\n", path);
        return nullptr;
    }
    char *buffer = malloc(sizeof(char) * size + 1);
    fread(buffer, size, 1, file);
    fclose(file);
    *sz = size;
    return buffer;
}

GLuint mx_load_spv(GLenum type, const char *path) {
    size_t data_size = 0;
    auto data = mx_read_file(path, &data_size);
    if (data == nullptr) {
        fprintf(stderr, "SPV load failed: %s\n", path);
        return 0;
    }
    GLuint shader = glCreateShader(type);
    glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, data, data_size);
    free(data);
    glSpecializeShader(shader, "main", 0, nullptr, nullptr);
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[4096];
        glGetShaderInfoLog(shader, 4096, nullptr, log);
        fprintf(stderr, "Shader error (%s): %s\n", path, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint mx_create_shader_program(const char *vertex_spv, const char *fragment_spv) {
    GLuint vs = mx_load_spv(GL_VERTEX_SHADER, vertex_spv);
    GLuint fs = mx_load_spv(GL_FRAGMENT_SHADER, fragment_spv);
    if (!vs || !fs) {
        fprintf(stderr, "mx_create_shader: Shader creation failed\n");
        if (vs)
            glDeleteShader(vs);
        if (fs)
            glDeleteShader(fs);
        return 0;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[4096];
        glGetProgramInfoLog(program, 4096, nullptr, log);
        fprintf(stderr, "Program link error: %s\n", log);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

GLuint mx_create_compute_program(const char *compute_spv) {
    GLuint c_shader = mx_load_spv(GL_COMPUTE_SHADER, compute_spv);
    GLuint program = glCreateProgram();
    glAttachShader(program, c_shader);
    glLinkProgram(program);
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        fprintf(stderr, "Error creating compute program: %s\n", compute_spv);
        glDeleteShader(c_shader);
        glDeleteProgram(program);
        return 0;
    }
    glDeleteShader(c_shader);
    return program;
}
