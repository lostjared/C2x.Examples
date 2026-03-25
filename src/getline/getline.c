#include "getline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *f_getline(char buffer[static 1], int bytes, FILE *fptr) {
    buffer[0] = 0;
    char *ret = fgets(buffer, bytes, fptr);
    if (ret) {
        char *position = strchr(buffer, '\n');
        if (position)
            *position = 0;
        else
            ret = nullptr;
    }
    return ret;
}
