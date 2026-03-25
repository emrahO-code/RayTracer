#ifndef RAYTRACER_SURFACE_H
#define RAYTRACER_SURFACE_H

#include <stdbool.h>
#include "ray.h"
#include "surface_record.h"
#include "interval.h"

typedef struct Surface Surface;

struct Surface {
    bool (*hit)(const Surface *self, const Ray r, interval t, surface_record *rec);
};
#endif //RAYTRACER_SURFACE_H