#include <stdio.h>
#include <stdlib.h>

void sort(int data[static 2], unsigned int s);
void swap(int *a, int *b);
void print_array(int data[static 1], unsigned int s);

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

void sort(int data[static 2], unsigned int s) {
    if (s < 2)
        return;
    bool swapped = false;
    unsigned int len = s;
    do {
        swapped = false;
        for (unsigned int i = 1; i <= len - 1; ++i) {
            if (data[i - 1] > data[i]) {
                swap(&data[i - 1], &data[i]);
                swapped = true;
            }
        }
        len--;
    } while (swapped && len > 1);
}

void print_array(int arr[static 1], unsigned int sz) {
    for (unsigned int i = 0; i < sz; ++i) {
        printf("%d: %d\n", i, arr[i]);
    }
}
