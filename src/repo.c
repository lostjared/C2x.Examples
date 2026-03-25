#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int by2(int x, int y) [[reproducible]];    // does not read globals
int by2_seq(int x, int y) [[unsequenced]]; // so unsequenced could be used

int main(void) {
    int x = 2;
    int y = 2;
    int value1 = by2(x, y);
    int value2 = by2(x, y);
    assert(value1 == value2);
    printf("value %d == %d\n", value1, value2);
    value1 = by2_seq(x, y);
    value2 = by2_seq(x, y);
    assert(value1 == value2);
    printf("value %d == %d\n", value1, value2);
    return 0;
}

int by2(int x, int y) [[reproducible]] {
    return x * y;
}

int by2_seq(int x, int y) [[unsequenced]] {
    return x * y;
}
