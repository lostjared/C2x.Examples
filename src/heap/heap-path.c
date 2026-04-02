#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const int INFINITE = 1e9;

typedef struct {
    int vertex;
    int distance;
} GNode;

int compare(const void *a, const void *b) {
    const GNode *ia = a;
    const GNode *ib = b;
    if (ia->distance < ib->distance)
        return 1;
    if (ia->distance > ib->distance)
        return -1;
    return 0;
}

typedef struct {
    int target;
    int weight;
} Edge;

typedef struct {
    Edge *e;
    int ec;
} Node;

bool dijkstra(const Node *graph, size_t num, int start, int destination) {
    if (graph == nullptr || num == 0 || (size_t)start >= num || destination < 0 || (size_t)destination >= num) {
        fprintf(stderr, "Error invalid graph dimensions.\n");
        return false;
    }
    int *dist = malloc(num * sizeof(int));
    int *prev = malloc(num * sizeof(int));
    for (size_t i = 0; i < num; ++i) {
        dist[i] = INFINITE;
        prev[i] = -1;
    }
    Heap heap;
    if (!heap_init(&heap, compare, free)) {
        fprintf(stderr, "Error on init of heap.\n");
        return false;
    }
    dist[start] = 0;
    GNode *start_node = malloc(sizeof(GNode));
    start_node->vertex = start;
    start_node->distance = 0;
    if (!heap_insert(&heap, start_node)) {
        fprintf(stderr, "Error on insert.\n");
        free(start_node);
        free(dist);
        free(prev);
        heap_destroy(&heap);
        return false;
    }
    while (heap.size > 0) {
        void *cur = nullptr;
        if (!heap_extract(&heap, &cur)) {
            fprintf(stderr, "Error on extract.\n");
            free(dist);
            free(prev);
            heap_destroy(&heap);
            return false;
        }
        GNode *current = cur;
        int u = current->vertex;
        int d = current->distance;
        free(current);
        if (d > dist[u])
            continue;
        for (int i = 0; i < graph[u].ec; ++i) {
            int v = graph[u].e[i].target;
            int weight = graph[u].e[i].weight;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                GNode *next = malloc(sizeof(GNode));
                next->vertex = v;
                next->distance = dist[v];
                if (!heap_insert(&heap, next)) {
                    fprintf(stderr, "Error on insert.\n");
                    free(next);
                    free(dist);
                    free(prev);
                    heap_destroy(&heap);
                    return false;
                }
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
        printf("\n");
        for (size_t i = 0; i < num; ++i) {
            printf("Distance to node %zu: %d\n", i, dist[i]);
        }
    }
    free(dist);
    free(prev);
    heap_destroy(&heap);
    return true;
}

Node *generate_test_graph(size_t num, unsigned int seed) {
    srand(seed);
    Node *graph = malloc(num * sizeof(Node));
    if (!graph)
        return nullptr;
    for (size_t i = 0; i < num; ++i) {
        int ec = (i == num - 1) ? 0 : (rand() % 4) + 2;
        graph[i].ec = ec;
        if (ec > 0) {
            graph[i].e = malloc((size_t)ec * sizeof(Edge));
            graph[i].e[0] = (Edge){(int)i + 1, (rand() % 10) + 1};
            for (int j = 1; j < ec; ++j) {
                int target = rand() % (int)num;
                int weight = (rand() % 20) + 1;
                graph[i].e[j] = (Edge){target, weight};
            }
        } else {
            graph[i].e = nullptr;
        }
    }
    return graph;
}

void free_test_graph(Node *graph, size_t num) {
    if (!graph)
        return;
    for (size_t i = 0; i < num; ++i) {
        free(graph[i].e);
    }
    free(graph);
}

int main(void) {
    size_t node_count = 50;
    int start_node = 0;
    int target_node = 49;
    Node *large_graph = generate_test_graph(node_count, (unsigned int)time(nullptr));
    if (!large_graph) {
        fprintf(stderr, "Failed to allocate test graph.\n");
        return EXIT_FAILURE;
    }
    if (!dijkstra(large_graph, node_count, start_node, target_node)) {
        fprintf(stderr, "Error dijkstra algorithm failed.\n");
        free_test_graph(large_graph, node_count);
        return EXIT_FAILURE;
    }
    free_test_graph(large_graph, node_count);
    return EXIT_SUCCESS;
}
