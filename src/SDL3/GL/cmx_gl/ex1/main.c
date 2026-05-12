
#include <cmx_gl.h>
#include <stdio.h>
#include <stdlib.h>

static constexpr int WIDTH = 1280;
static constexpr int HEIGHT = 720;

float vertices[] = {
    0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f};

int main(void) {
    printf("Example1: [Start up]\n");
    struct mx_app_info app = {};
    if (!mx_init_sdl(&app, WIDTH, HEIGHT)) {
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
    GLuint program = mx_create_shader_program("vert.spv", "frag.spv");
    if (program == 0) {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        SDL_GL_DestroyContext(app.ctx);
        SDL_DestroyWindow(app.window);
        SDL_Quit();
        printf("Example 1: [Failure].\n");
        return EXIT_FAILURE;
    }
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
    printf("Example 1: [Shut down]\n");
    return mx_close_sdl(&app);
}
