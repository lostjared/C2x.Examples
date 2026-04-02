#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(void) {
    Matrix mat;
    if (!init_matrix(&mat, 5)) {
        fprintf(stderr, "Error on init.\n");
        return EXIT_FAILURE;
    }
    ADD_EDGE(mat, 0, 1, 4);
    ADD_EDGE(mat, 0, 2, 1);
    ADD_EDGE(mat, 2, 3, 8);
    ADD_EDGE(mat, 3, 4, 5);
    print(&mat);
    free_matrix(&mat);
    return EXIT_SUCCESS;
}
