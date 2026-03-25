#ifndef RAYTRACER_SURFACE_H
#define RAYTRACER_SURFACE_H

#include <stdbool.h>
#include "ray.h"
#include "surface_record.h"

typedef struct Surface Surface;

struct Surface {
    bool (*hit)(const Surface *self, const Ray r, double t_min, double t_max, surface_record *rec);
};
#endif //RAYTRACER_SURFACE_H