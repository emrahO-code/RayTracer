//
// Created by Emrah Omerovic on 3/23/26.
//

#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "vec3.h"

typedef struct {
    Vec3 origin;
    Vec3 direction;
}Ray;

static Ray ray(Vec3 origin, Vec3 direction) {
    return (Ray){origin,direction};
}

static Vec3 ray_at(Ray r, double t) {
    return vec3_add(r.origin, vec3_scale(r.direction , t));
}
#endif //RAYTRACER_RAY_H