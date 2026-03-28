#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>

typedef struct _MXObject {
    char *name;
    bool (*init)(struct _MXObject *obj, const char *name);
    void (*say_hello)(struct _MXObject *obj, const char *message);
} MXObject;

bool init_object(MXObject **obj);
void release_object(MXObject **obj);

#endif
