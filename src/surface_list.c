#include "surface_list.h"
#include <stdlib.h>

#define INITIAL_CAP 8

static bool surface_list_hit(const Surface *self, const Ray r,double t_min, double t_max, surface_record *rec) {
    const Surface_list *list = (const Surface_list *)self;
    surface_record temp;
    bool hit_anything = false;
    double closest = t_max;
    for (int i=0; i<list->count;i++) {
        if (list->objects[i]->hit(list->objects[i], r, t_min, closest, &temp )) {
            hit_anything = true;
            closest = temp.t;
            *rec = temp;
        }
    }
    return hit_anything;

}
Surface_list surface_list_create() {
    Surface_list list;
    list.base.hit = surface_list_hit;
    list.count = 0;
    list.capacity = INITIAL_CAP;
    list.objects = malloc(INITIAL_CAP* sizeof(Surface *));
    return list;
}

void surface_list_add(Surface_list *list, Surface *s) {
    if (list->count >= list-> capacity) {
        list->capacity *= 2;
        list->objects = realloc(list->objects, list->capacity * sizeof(Surface *));
    }
    list->objects[list->count++] = s;
}

void surface_list_free(Surface_list *list) {
    free(list->objects);
    list->objects = NULL;
    list->capacity = 0;
    list->count = 0;
}




