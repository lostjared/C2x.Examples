#include "object.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    MXObject *obj = nullptr;
    if (init_object(&obj)) {
        if (obj->init(obj, "Jared")) {
            obj->say_hello(obj, "Hello, World");
        } else {
            release_object(&obj);
            fprintf(stderr, "Error on init.\n");
            return EXIT_FAILURE;
        }
        if (obj->init(obj, "Mr. Bruni")) {
            obj->say_hello(obj, "Hey whats up?");
            release_object(&obj);
        } else {
            release_object(&obj);
            fprintf(stderr, "Error on init.\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Error: failed to init.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
