
#include <cmx_gl.h>
#include <stdio.h>
#include <stdlib.h>

static constexpr int APP_WIDTH = 1280;
static constexpr int APP_HEIGHT = 720;

float vertices[] = {
    -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f};

uint32_t indices[] = {0, 1, 2, 0, 2, 3};

int main(int argc, char *argv[argc + 1]) {
    if (argc != 3) {
        fprintf(stderr, "%s: <compute.spv> <image.bmp>\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct mx_app_info app = {};
    if (!mx_init_sdl(&app, "Hello, World with Compute Shader!", APP_WIDTH, APP_HEIGHT)) {
        return EXIT_FAILURE;
    }
    SDL_Surface *img = SDL_LoadBMP(argv[2]);
    if (!img) {
        mx_close_sdl(&app);
        return EXIT_FAILURE;
    }
    printf("Image loaded: %ux%u\n", img->w, img->h);
    SDL_Event e;
    bool active = true;
    GLuint vao, vbo, ebo;

    uint32_t img_width = img->w;
    uint32_t img_height = img->h;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    GLuint input_texture;
    glGenTextures(1, &input_texture);
    glBindTexture(GL_TEXTURE_2D, input_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    SDL_Surface *rgba_img = SDL_ConvertSurface(img, SDL_PIXELFORMAT_ABGR8888);

    if (!rgba_img) {
        fprintf(stderr, "Failed to convert image to RGBA\n");
        glDeleteTextures(1, &input_texture);
        SDL_DestroySurface(img);
        mx_close_sdl(&app);
        return EXIT_FAILURE;
    }

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, img_width, img_height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img_width, img_height, GL_RGBA, GL_UNSIGNED_BYTE, rgba_img->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    MX_CHECK_GL_ERROR();

    SDL_DestroySurface(img);
    SDL_DestroySurface(rgba_img);

    GLuint program = mx_create_shader_program("vert.spv", "frag.spv");
    GLuint compute_program = mx_create_compute_program(argv[1]);

    if (program == 0 || compute_program == 0) {
        glDeleteTextures(1, &input_texture);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);
        mx_close_sdl(&app);
        printf("Example 2: [Failure]\n");
        return EXIT_FAILURE;
    }

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

        glUseProgram(compute_program);
        glBindImageTexture(0, input_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glDispatchCompute((img_width + 15) / 16, (img_height + 15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        MX_CHECK_GL_ERROR();

        glViewport(0, 0, app.w, app.h);
        glClearColor(0.1f, 0.0f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, input_texture);
        glUniform1i(glGetUniformLocation(program, "screenTex"), 0);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        SDL_GL_SwapWindow(app.window);
    }

    glDeleteProgram(program);
    glDeleteProgram(compute_program);
    glDeleteTextures(1, &input_texture);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    mx_close_sdl(&app);
    printf("Example 2: [Success]\n");
    return EXIT_SUCCESS;
}
