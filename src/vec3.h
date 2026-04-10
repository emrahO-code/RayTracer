#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdlib.h>

#include "interval.h"

typedef struct {
    double x,y,z;
}Vec3;

//Basic Constructor and Basic Scalar Multiplication and Vector Addition
static Vec3 vec3(const double x, const double y,const double z) {
    return (Vec3){x,y,z};
}

static Vec3 vec3_add(const Vec3 a, const Vec3 b) {
    return (Vec3){a.x+b.x,a.y+b.y,a.z+b.z};
}

static Vec3 vec3_subtract(const Vec3 a, const Vec3 b) {
    return (Vec3){a.x-b.x, a.y-b.y, a.z-b.z};
}

static Vec3 vec3_scale(const Vec3 v, const double c) {
    return (Vec3){c * v.x,c * v.y , c * v.z};
}

static Vec3 vec3_negate(const Vec3 v) {
    return (Vec3){-v.x, -v.y, -v.z};
}

static double vec3_dot(const Vec3 a, const Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

static Vec3 vec3_cross(const Vec3 a, const Vec3 b) {
    return (Vec3){a.y*b.z - a.z * b.y , a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

//Length-based functions
static double vec3_length_sq(const Vec3 a) {
    return vec3_dot(a,a);
}

static double vec3_length(const Vec3 a) {
    return sqrt(vec3_length_sq(a));
}

static Vec3 vec3_normalize(const Vec3 a) {
    return vec3_scale(a, 1.0/vec3_length(a));
}

//Random-based helper functions

static Vec3 vec3_random() {
    return (Vec3){rand()/(RAND_MAX+1.0),rand()/(RAND_MAX+1.0),rand()/(RAND_MAX+1.0)};
}

static Vec3 vec3_random_interval(interval Int) {
    return vec3_add( (Vec3) {Int.min,Int.min,Int.min} ,vec3_scale(vec3_random(),(Int.max -Int.min )));
}

static Vec3 vec3_random_unit_vector() {
    for (;;) {
        const Vec3 a = vec3_random_interval((interval) {-1,1});
        const double a_dot = vec3_length_sq(a);
        if (1e-160 < a_dot && a_dot <= 1) {
            return vec3_scale(a, 1/a_dot);
        }
    }
}

//Diffuse-helper functions
static Vec3 vec3_random_on_hemisphere(const Vec3 normal) {
    Vec3 on_unit_sphere = vec3_random_unit_vector();
    if (vec3_dot(on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    }
    return vec3_scale(on_unit_sphere, -1);
}



#endif