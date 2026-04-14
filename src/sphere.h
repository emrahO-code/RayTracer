#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "surface.h"
#include "vec3.h"

typedef struct Sphere {
    Surface base;
    Vec3 center;
    double radius;
    const Material *mat;
}Sphere;

Sphere sphere_create(Vec3 center, double radius, const Material *mat);

#endif //RAYTRACER_SPHERE_H