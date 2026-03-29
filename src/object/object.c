#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool object_init(MXObject *obj, const char *name) {
    if (obj == nullptr || name == nullptr)
        return false;
    char *temp;
    size_t len = strlen(name);
    temp = malloc(len + 1);
    if (temp == nullptr)
        return false;
    memcpy(temp, name, len + 1);
    free(obj->name);
    obj->name = temp;
    return true;
}

void object_say_hello(MXObject *obj, const char *message) {
    if (obj == nullptr || obj->name == nullptr || message == nullptr)
        return;
    printf("%s: %s\n", obj->name, message);
}

bool init_object(MXObject **obj) {
    if (obj == nullptr)
        return false;

    if (*obj != nullptr) {
        fprintf(stderr, "Error already initalized.\n");
        return false;
    }

    MXObject *o = malloc(sizeof(MXObject));
    if (o == nullptr)
        return false;
    memset(o, 0, sizeof(MXObject));
    o->init = object_init;
    o->say_hello = object_say_hello;
    *obj = o;
    return true;
}

void release_object(MXObject **obj) {
    if (obj != nullptr && *obj != nullptr) {
        free((*obj)->name);
        (*obj)->name = nullptr;
        free(*obj);
        *obj = nullptr;
    }
}
