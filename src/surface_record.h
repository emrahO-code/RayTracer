
#ifndef RAYTRACER_HIT_RECORD_H
#define RAYTRACER_HIT_RECORD_H

#include "vec3.h"
#include "ray.h"

typedef struct {
    Vec3 p;
    Vec3 normal;
    double t;
    bool front_face;
}surface_record;


static void hit_record_set_face_normal(surface_record *rec, Ray r, Vec3 outward_normal){
    rec->front_face = vec3_dot(r.direction,outward_normal) < 0;
    rec->normal = rec->front_face ? outward_normal : vec3_negate(outward_normal);
}
#endif