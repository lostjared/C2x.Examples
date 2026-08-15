#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Enter integers 0 to exit:");
    bool active = true;
    size_t total = 0;
    while (active) {
        int result = 0;
        while (result == 0) {
            int ret = 0;
            if ((ret = scanf("%d", &result)) > 0)
                printf("Total is: %zu\n", (total += result));
            else if (ret == 0 || ret == EOF) {
                char c = 0;
                while ((c = getchar()) != EOF && c != '\n')
                    continue;
            } else
                break;
            if (result == 0)
                return EXIT_SUCCESS;
        }
        if (result == 0)
            break;
    }
    return EXIT_SUCCESS;
}
