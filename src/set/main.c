#include "set.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void destroy(void *ptr) {
    free(ptr);
}

int compare(const void *a, const void *b) {
    const size_t *one = a;
    const size_t *two = b;
    if (*one < *two)
        return 1;
    else if (*one > *two)
        return -1;
    return 0;
}

void echo(const void *ptr) {
    if (ptr == nullptr)
        return;

    const size_t *s = ptr;
    printf("value is: %zu\n", *s);
}

int main(void) {
    Set *set_value1 = nullptr;
    if (!set_init(&set_value1, destroy, compare)) {
        fprintf(stderr, "Error allcoating set.\n");
        return EXIT_FAILURE;
    }
    size_t data = 25;
    if (!set_insert(set_value1, &data, sizeof(data))) {
        fprintf(stderr, "Error on insert.\n");
        set_free(set_value1);
        return EXIT_FAILURE;
    }
    data = 50;

    if (!set_insert(set_value1, &data, sizeof(data))) {
        fprintf(stderr, "Error on insert.\n");
        set_free(set_value1);
        return EXIT_FAILURE;
    }

    if (!set_insert(set_value1, &data, sizeof(data))) {
        fprintf(stderr, "Error on insert.\n");
        set_free(set_value1);
        return EXIT_FAILURE;
    }
    data = 255;
    if (!set_insert(set_value1, &data, sizeof(data))) {
        fprintf(stderr, "Error on insert.\n");
        set_free(set_value1);
        return EXIT_FAILURE;
    }
    printf("with value 255:\n");
    set_print(set_value1, echo);
    if (!set_remove(set_value1, &data)) {
        fprintf(stderr, "Error on remove.\n");
        set_free(set_value1);
        return EXIT_FAILURE;
    }

    printf("After remove:\n");
    set_print(set_value1, echo);

    Set *set_value2 = nullptr;
    if (!set_init(&set_value2, destroy, compare)) {
        fprintf(stderr, "Error allcoating set.\n");
        set_free(set_value1);
        return EXIT_FAILURE;
    }
    data = 251;
    if (!set_insert(set_value2, &data, sizeof(data))) {
        fprintf(stderr, "Error on insert.\n");
        set_free(set_value2);
        set_free(set_value1);
        return EXIT_FAILURE;
    }
    data = 50;
    if (!set_insert(set_value2, &data, sizeof(data))) {
        fprintf(stderr, "Error on insert.\n");
        set_free(set_value2);
        set_free(set_value1);
        return EXIT_FAILURE;
    }
    printf("Set #2\n");
    set_print(set_value2, echo);
    Set *set_u = nullptr;
    if (!set_union(&set_u, set_value1, set_value2, destroy, compare)) {
        set_free(set_value1);
        set_free(set_value2);
        fprintf(stderr, "Error on union of two sets.\n");
        return EXIT_FAILURE;
    }
    printf("union of two sets.\n");
    set_print(set_u, echo);

    Set *set_i = nullptr;
    if (!set_intersection(&set_i, set_value1, set_value2, destroy, compare)) {
        set_free(set_value1);
        set_free(set_value2);
        set_free(set_u);
        fprintf(stderr, "Error on intersection.\n");
        return EXIT_FAILURE;
    }
    printf("Intersection: \n");
    set_print(set_i, echo);
    Set *set_d = nullptr;
    if (!set_difference(&set_d, set_value1, set_value2, destroy, compare)) {
        set_free(set_value1);
        set_free(set_value2);
        set_free(set_i);
        set_free(set_u);
        fprintf(stderr, "Error on diference.\n");
        return EXIT_FAILURE;
    }
    printf("Set1: {\n");
    set_print(set_value1, echo);
    printf("}\n");
    printf("Set2: {\n");
    set_print(set_value2, echo);
    printf("}\n");
    Set *sub_i = nullptr;
    if (!set_init(&sub_i, destroy, compare)) {
        fprintf(stderr, "Error on init of set.\n");
        set_free(set_d);
        set_free(set_i);
        set_free(set_value1);
        set_free(set_value2);
        set_free(set_u);
        return EXIT_FAILURE;
    }

    data = 25;
    if (!set_insert(sub_i, &data, sizeof(data))) {
        set_free(set_d);
        set_free(set_i);
        set_free(set_value1);
        set_free(set_value2);
        set_free(set_u);
        return EXIT_FAILURE;
    }
    printf("Set3: (Sub) {\n");
    set_print(sub_i, echo);
    printf("}\n");

    printf("Difference: {\n");
    set_print(set_d, echo);
    printf("}\n");
    if (!set_is_subset(set_value1, set_value2)) {
        printf("Set value1 is not a subset of set value2.\n");
    } else {
        printf("Set value` is a subset of set value2.\n");
    }
    if (!set_is_subset(sub_i, set_value1)) {
        printf("Set sub_i, is not a subset of set value 1\n");
    } else {
        printf("Set sub_i, is a subset of set value 1\n");
    }
    if (!set_is_equal(set_value1, set_value2)) {
        printf("Set value1 not equal to set value2\n");
    } else {
        printf("Set value1 is equal to set value2\n");
    }
    set_free(sub_i);
    set_free(set_d);
    set_free(set_i);
    set_free(set_value1);
    set_free(set_value2);
    set_free(set_u);
    return EXIT_SUCCESS;
}
