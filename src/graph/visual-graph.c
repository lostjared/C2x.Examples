#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static constexpr int WINDOW_WIDTH = 1280;
static constexpr int WINDOW_HEIGHT = 720;
static constexpr int NODE_RADIUS = 14;
static constexpr int FONT_SIZE = 13;
static constexpr int FONT_SIZE_LG = 20;
static constexpr int PATH_THICKNESS = 3;
static constexpr int INFINITE_DIST = 1000000000;

typedef struct {
    int *matrix;
    size_t num;
} Matrix;

enum {
    NO_EDGE = -1
};

bool init_matrix(Matrix *matrix, size_t num) {
    if (matrix == nullptr || num == 0)
        return false;
    matrix->num = num;
    int *mat = malloc(num * num * sizeof(int));
    if (mat == nullptr)
        return false;
    matrix->matrix = mat;
    for (size_t i = 0; i < num * num; ++i)
        mat[i] = NO_EDGE;
    for (size_t i = 0; i < num; ++i)
        mat[i * num + i] = 0;
    return true;
}

void free_matrix(Matrix *matrix) {
    if (matrix == nullptr)
        return;
    free(matrix->matrix);
}

bool add_edge(Matrix *matrix, size_t from, size_t to, int weight, bool directed) {
    if (matrix == nullptr || matrix->matrix == nullptr)
        return false;
    if (from >= matrix->num || to >= matrix->num)
        return false;
    matrix->matrix[from * matrix->num + to] = weight;
    if (!directed)
        matrix->matrix[to * matrix->num + from] = weight;
    return true;
}

int get_weight(const Matrix *matrix, size_t from, size_t to) {
    if (matrix == nullptr || matrix->matrix == nullptr)
        return NO_EDGE;
    return matrix->matrix[from * matrix->num + to];
}

typedef struct {
    int *path;
    int path_len;
    int total_dist;
} PathResult;

PathResult dijkstra(const Matrix *graph, int start, int destination) {
    PathResult result = {nullptr, 0, -1};
    if (graph == nullptr || graph->matrix == nullptr)
        return result;

    size_t num = graph->num;
    int *dist = malloc(num * sizeof(int));
    int *prev = malloc(num * sizeof(int));
    bool *visited = malloc(num * sizeof(bool));
    if (dist == nullptr || prev == nullptr || visited == nullptr) {
        free(dist);
        free(prev);
        free(visited);
        return result;
    }

    for (size_t i = 0; i < num; ++i) {
        dist[i] = INFINITE_DIST;
        prev[i] = -1;
        visited[i] = false;
    }
    dist[start] = 0;

    for (size_t count = 0; count < num; ++count) {
        int min_dist = INFINITE_DIST;
        int u = -1;
        for (size_t i = 0; i < num; ++i) {
            if (!visited[i] && dist[i] <= min_dist) {
                min_dist = dist[i];
                u = (int)i;
            }
        }
        if (u == -1 || min_dist == INFINITE_DIST)
            break;
        visited[u] = true;
        for (size_t v = 0; v < num; ++v) {
            int weight = get_weight(graph, (size_t)u, v);
            if (!visited[v] && weight != NO_EDGE && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
            }
        }
    }

    if (dist[destination] != INFINITE_DIST) {
        result.total_dist = dist[destination];
        int len = 0;
        for (int c = destination; c != -1; c = prev[c])
            len++;
        result.path = malloc((size_t)len * sizeof(int));
        if (result.path == nullptr) {
            free(dist);
            free(prev);
            free(visited);
            return result;
        }
        result.path_len = len;
        int idx = len - 1;
        for (int c = destination; c != -1; c = prev[c])
            result.path[idx--] = c;
    }

    free(dist);
    free(prev);
    free(visited);
    return result;
}

bool generate_test_matrix(Matrix *mat, size_t num, unsigned int seed) {
    srand(seed);
    if (!init_matrix(mat, num))
        return false;
    for (size_t i = 0; i < num; ++i) {
        if (i < num - 1)
            add_edge(mat, i, i + 1, (rand() % 10) + 1, false);
        int extra = rand() % 3;
        for (int j = 0; j < extra; ++j) {
            size_t target = (size_t)rand() % num;
            if (target != i)
                add_edge(mat, i, target, (rand() % 20) + 1, false);
        }
    }
    return true;
}

typedef struct {
    float x, y;
} Vec2;

static void compute_circle_layout(Vec2 *pos, size_t num, float cx, float cy, float r) {
    for (size_t i = 0; i < num; ++i) {
        float angle = 2.0f * (float)M_PI * (float)i / (float)num - (float)M_PI / 2.0f;
        pos[i].x = cx + r * cosf(angle);
        pos[i].y = cy + r * sinf(angle);
    }
}

static void draw_filled_circle(SDL_Renderer *ren, int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; dy++) {
        int dx = (int)sqrtf((float)(radius * radius - dy * dy));
        SDL_RenderLine(ren, (float)(cx - dx), (float)(cy + dy), (float)(cx + dx), (float)(cy + dy));
    }
}

static void draw_thick_line(SDL_Renderer *ren, int x1, int y1, int x2, int y2, int thickness) {
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 1.0f)
        return;
    float nx = -dy / len;
    float ny = dx / len;
    for (int i = -thickness / 2; i <= thickness / 2; i++) {
        SDL_RenderLine(ren,
                       (float)(x1 + (int)(nx * (float)i)), (float)(y1 + (int)(ny * (float)i)),
                       (float)(x2 + (int)(nx * (float)i)), (float)(y2 + (int)(ny * (float)i)));
    }
}

static void render_text(SDL_Renderer *ren, TTF_Font *font, const char *text,
                        int x, int y, SDL_Color color) {
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, 0, color);
    if (surf == nullptr)
        return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    if (tex != nullptr) {
        SDL_FRect dst = {(float)x, (float)y, (float)surf->w, (float)surf->h};
        SDL_RenderTexture(ren, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_DestroySurface(surf);
}

static void render_text_centered(SDL_Renderer *ren, TTF_Font *font, const char *text,
                                 int cx, int cy, SDL_Color color) {
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, 0, color);
    if (surf == nullptr)
        return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    if (tex != nullptr) {
        SDL_FRect dst = {(float)(cx - surf->w / 2), (float)(cy - surf->h / 2), (float)surf->w, (float)surf->h};
        SDL_RenderTexture(ren, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_DestroySurface(surf);
}

int main(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    if (!TTF_Init()) {
        fprintf(stderr, "TTF_Init failed: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("Dijkstra Shortest Path",
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == nullptr) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    TTF_Font *font = TTF_OpenFont("font.ttf", FONT_SIZE);
    if (font == nullptr) {
        fprintf(stderr, "Failed to load font.ttf: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    TTF_Font *font_lg = TTF_OpenFont("font.ttf", FONT_SIZE_LG);
    if (font_lg == nullptr)
        font_lg = font;

    size_t node_count = 30;
    int start_node = 0;
    int target_node = (int)node_count - 1;
    unsigned int seed = (unsigned int)time(nullptr);

    Matrix mat;
    if (!generate_test_matrix(&mat, node_count, seed)) {
        fprintf(stderr, "Failed to generate graph.\n");
        if (font_lg != font)
            TTF_CloseFont(font_lg);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    PathResult path = dijkstra(&mat, start_node, target_node);
    bool *on_path = calloc(node_count, sizeof(bool));
    bool *path_edge = calloc(node_count * node_count, sizeof(bool));
    Vec2 *positions = malloc(node_count * sizeof(Vec2));
    if (on_path == nullptr || path_edge == nullptr || positions == nullptr) {
        fprintf(stderr, "Failed to allocate memory.\n");
        free(on_path);
        free(path_edge);
        free(positions);
        free(path.path);
        free_matrix(&mat);
        if (font_lg != font)
            TTF_CloseFont(font_lg);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    if (path.path != nullptr && path.path_len > 0) {
        for (int i = 0; i < path.path_len; i++)
            on_path[path.path[i]] = true;
        for (int i = 0; i < path.path_len - 1; i++) {
            size_t a = (size_t)path.path[i];
            size_t b = (size_t)path.path[i + 1];
            path_edge[a * node_count + b] = true;
            path_edge[b * node_count + a] = true;
        }
    }
    float cx = WINDOW_WIDTH / 2.0f;
    float cy = WINDOW_HEIGHT / 2.0f + 20.0f;
    float layout_r = fminf(cx, cy) - 60.0f;
    compute_circle_layout(positions, node_count, cx, cy, layout_r);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color cyan_c = {0, 255, 255, 255};

    bool running = true;
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT)
                running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_ESCAPE)
                    running = false;
                if (ev.key.key == SDLK_R) {
                    free_matrix(&mat);
                    free(path.path);
                    free(on_path);
                    free(path_edge);

                    seed = (unsigned int)time(nullptr) ^ (unsigned int)rand();
                    if (!generate_test_matrix(&mat, node_count, seed)) {
                        fprintf(stderr, "Failed to regenerate graph.\n");
                        running = false;
                        break;
                    }
                    path = dijkstra(&mat, start_node, target_node);

                    on_path = calloc(node_count, sizeof(bool));
                    path_edge = calloc(node_count * node_count, sizeof(bool));
                    if (on_path == nullptr || path_edge == nullptr) {
                        fprintf(stderr, "Failed to allocate memory.\n");
                        running = false;
                        break;
                    }
                    if (path.path != nullptr && path.path_len > 0) {
                        for (int i = 0; i < path.path_len; i++)
                            on_path[path.path[i]] = true;
                        for (int i = 0; i < path.path_len - 1; i++) {
                            size_t a = (size_t)path.path[i];
                            size_t b = (size_t)path.path[i + 1];
                            path_edge[a * node_count + b] = true;
                            path_edge[b * node_count + a] = true;
                        }
                    }
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 25, 25, 30, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 55, 55, 65, 255);
        for (size_t i = 0; i < node_count; ++i) {
            for (size_t j = i + 1; j < node_count; ++j) {
                int w = get_weight(&mat, i, j);
                if (w != NO_EDGE && w != 0 && !path_edge[i * node_count + j]) {
                    SDL_RenderLine(renderer,
                                   positions[i].x, positions[i].y,
                                   positions[j].x, positions[j].y);
                }
            }
        }
        if (path.path != nullptr && path.path_len > 1) {
            SDL_SetRenderDrawColor(renderer, 0, 230, 180, 255);
            for (int i = 0; i < path.path_len - 1; i++) {
                int a = path.path[i];
                int b = path.path[i + 1];
                draw_thick_line(renderer,
                                (int)positions[a].x, (int)positions[a].y,
                                (int)positions[b].x, (int)positions[b].y,
                                PATH_THICKNESS);
                int w = get_weight(&mat, (size_t)a, (size_t)b);
                int mx = ((int)positions[a].x + (int)positions[b].x) / 2;
                int my = ((int)positions[a].y + (int)positions[b].y) / 2;
                char buf[16];
                snprintf(buf, sizeof(buf), "%d", w);
                render_text_centered(renderer, font, buf, mx, my - 12, yellow);
            }
        }
        for (size_t i = 0; i < node_count; ++i) {
            int nx = (int)positions[i].x;
            int ny = (int)positions[i].y;
            if ((int)i == start_node)
                SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255);
            else if ((int)i == target_node)
                SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
            else if (on_path[i])
                SDL_SetRenderDrawColor(renderer, 0, 200, 180, 255);
            else
                SDL_SetRenderDrawColor(renderer, 70, 130, 200, 255);
            draw_filled_circle(renderer, nx, ny, NODE_RADIUS);

            char label[8];
            snprintf(label, sizeof(label), "%zu", i);
            render_text_centered(renderer, font, label, nx, ny, white);
        }
        char info[256];
        if (path.total_dist >= 0)
            snprintf(info, sizeof(info),
                     "Shortest Path: %d -> %d  |  Distance: %d  |  R = regenerate  |  ESC = quit",
                     start_node, target_node, path.total_dist);
        else
            snprintf(info, sizeof(info),
                     "No path from %d to %d  |  R = regenerate  |  ESC = quit",
                     start_node, target_node);
        render_text(renderer, font_lg, info, 10, 10, cyan_c);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    free(positions);
    free(on_path);
    free(path_edge);
    free(path.path);
    free_matrix(&mat);
    if (font_lg != font)
        TTF_CloseFont(font_lg);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
