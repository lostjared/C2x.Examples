#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const int INFINITE = 1e9;

typedef struct {
    int *matrix;
    size_t num;
} Matrix;

enum {
    NO_EDGE = -1,
    EDGE = 1
};

#define ADD_EDGE(mat, from, to, weight)           \
    if (!add_edge(&mat, from, to, weight, false)) \
        return EXIT_FAILURE;

bool init_matrix(Matrix *matrix, size_t num) {
    if (matrix == nullptr || num == 0)
        return false;
    matrix->num = num;
    size_t sizeval = ((num * num) * sizeof(int));
    int *mat = malloc(sizeval);
    if (mat == nullptr)
        return false;
    matrix->matrix = mat;
    for (size_t i = 0; i < (num * num); ++i) {
        matrix->matrix[i] = NO_EDGE;
    }
    for (size_t i = 0; i < num; ++i) {
        matrix->matrix[i * num + i] = 0;
    }
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

    if (from >= matrix->num || to >= matrix->num) {
        fprintf(stderr, "Index out of range.\n");
        return false;
    }
    matrix->matrix[from * matrix->num + to] = weight;
    if (!directed) {
        matrix->matrix[to * matrix->num + from] = weight;
    }
    return true;
}

int get_weight(const Matrix *matrix, size_t from, size_t to) {
    if (matrix == nullptr || matrix->matrix == nullptr)
        return false;

    return matrix->matrix[from * matrix->num + to];
}

void print(Matrix *matrix) {
    if (matrix == nullptr || matrix->matrix == nullptr)
        return;
    printf("    ");
    for (size_t i = 0; i < matrix->num; ++i) {
        printf("%4zu ", i);
    }
    printf("\n");
    for (size_t i = 0; i < matrix->num; ++i) {
        printf("%2zu |", i);
        for (size_t z = 0; z < matrix->num; ++z) {
            int w = get_weight(matrix, i, z);
            if (w == NO_EDGE) {
                printf("   - ");
            } else {
                printf("%4d ", w);
            }
        }
        printf("\n");
    }
}


bool dijkstra_matrix(const Matrix *graph, int start, int destination) {
    if (graph == nullptr || graph->matrix == nullptr || start < 0 || (size_t)start >= graph->num || destination < 0 || (size_t)destination >= graph->num) {
        fprintf(stderr, "Invalid graph dimensions or target.\n");
        return false;
    }

    size_t num = graph->num;
    int *dist = malloc(num * sizeof(int));
    int *prev = malloc(num * sizeof(int));
    bool *visited = malloc(num * sizeof(bool));

    if (!dist || !prev || !visited) {
        free(dist);
       	free(prev); 
	free(visited);
        return false;
    }
    for (size_t i = 0; i < num; ++i) {
        dist[i] = INFINITE;
        prev[i] = -1;
        visited[i] = false;
    }

    dist[start] = 0;
    for (size_t count = 0; count < num; ++count) {
        int min_dist = INFINITE;
        int u = -1;

        for (size_t i = 0; i < num; ++i) {
            if (!visited[i] && dist[i] <= min_dist) {
                min_dist = dist[i];
                u = (int)i;
            }
        }

        if (u == -1 || min_dist == INFINITE) {
            break;
        }
 	visited[u] = true;
	for (size_t v = 0; v < num; ++v) {
            int weight = get_weight(graph, (size_t) u, v);
   	    if (!visited[v] && weight != NO_EDGE && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
            }
        }
    }

    if (dist[destination] == INFINITE) {
        printf("No path exists from %d to %d\n", start, destination);
    } else {
        printf("Path to %d:\n", destination);
        int current = destination;
        while (current != -1) {
            printf("%d ", current);
            current = prev[current];
        }
        printf("\nTotal distance: %d\n", dist[destination]);
    }

    free(dist);
    free(prev);
    free(visited);
    return true;
}

bool generate_test_matrix(Matrix *mat, size_t num, unsigned int seed) {
    srand(seed);
    if (!init_matrix(mat, num)) {
        return false;
    }

    for (size_t i = 0; i < num; ++i) {
        if (i < num - 1) {
            add_edge(mat, i, i + 1, (rand() % 10) + 1, false);
        }

        int extra_edges = rand() % 3;
        for (int j = 0; j < extra_edges; ++j) {
            size_t target = (size_t)rand() % num;
            if (target != i) {
                int weight = (int)(rand() % 20) + 1;
                add_edge(mat, i, target, weight, false);
            }
        }
    }
    return true;
}


int main(void) {
    size_t node_count = 50;
    int start_node = 0;
    int target_node = 49;
    Matrix mat;
    if (!generate_test_matrix(&mat, node_count, (unsigned int)time(nullptr))) {
        fprintf(stderr, "Failed to generate test matrix.\n");
        return EXIT_FAILURE;
    }
    if (!dijkstra_matrix(&mat, start_node, target_node)) {
        fprintf(stderr, "Error dijkstra algorithm failed.\n");
        free_matrix(&mat);
        return EXIT_FAILURE;
    }
    print(&mat);
    free_matrix(&mat);
    return EXIT_SUCCESS;
}
