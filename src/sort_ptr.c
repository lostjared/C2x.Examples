#include <stdio.h>
#include <stdlib.h>

void sort(int *data, size_t s);
void swap(int *a, int *b);
void print_array(int *data, unsigned int s);

int main() {
    int values[7] = {19, 2, 42, 100, 55, 1};
    sort(values, 6);
    print_array(values, 6);
    return 0;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void sort(int *data, size_t s) {
    if (s < 2)
        return;
    bool swapped = false;
    int *end = data + s;
    do {
        swapped = false;
        for (int *ptr = data; ptr < (end - 1); ++ptr) {
            if (*ptr > *(ptr + 1)) {
                swap(ptr, ptr + 1);
                swapped = true;
            }
        }
        end--;
    } while (swapped && (end - data) > 1);
}

void print_array(int *arr, unsigned int sz) {
    for (unsigned int i = 0; i < sz; ++i) {
        printf("%d: %d\n", i, arr[i]);
    }
}
