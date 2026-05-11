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

float vertices[] = {
    0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f};

static char *read_file(const char *path, size_t *sz) {
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

static GLuint load_spv(GLenum type, const char *path) {
    size_t data_size = 0;
    auto data = read_file(path, &data_size);
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

int main(void) {

    struct app_info app = {};

    if (!init_sdl(&app, WIDTH, HEIGHT)) {
        return EXIT_FAILURE;
    }
    SDL_Event e;
    bool active = true;
    GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    GLuint vs = load_spv(GL_VERTEX_SHADER, "vert.spv");
    GLuint fs = load_spv(GL_FRAGMENT_SHADER, "frag.spv");
    if (!vs || !fs) {
        fprintf(stderr, "Shader creation failed");
        SDL_GL_DestroyContext(app.ctx);
        SDL_DestroyWindow(app.window);
        SDL_Quit();
        return EXIT_FAILURE;
        ;
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
    glUseProgram(program);

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
        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        SDL_GL_SwapWindow(app.window);
    }

    glDeleteProgram(program);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    if (!SDL_GL_DestroyContext(app.ctx)) {
        fprintf(stderr, "Error destroying GL Context: %s\n", SDL_GetError());
        SDL_DestroyWindow(app.window);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

bool init_sdl(struct app_info *app, int width, int height) {
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
    const char *version = glGetString(GL_VERSION);
    if (version != nullptr) {
        printf("OpenGL Version: %s\n", version);
    }
    return true;
}
