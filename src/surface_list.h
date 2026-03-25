#ifndef RAYTRACER_SURFACE_LIST_H
#define RAYTRACER_SURFACE_LIST_H

#include "surface.h"

typedef struct {
    Surface base;
    Surface **objects;
    int count;
    int capacity;
} Surface_list;

Surface_list surface_list_create();
void surface_list_add(Surface_list *list, Surface *s);
void surface_list_free(Surface_list *list);

#endif //RAYTRACER_SURFACE_LIST_H