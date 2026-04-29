#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdlib.h>
#include "interval.h"
#include <stdbool.h>

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

static Vec3 vec3_random(unsigned int *seed) {
    return (Vec3){rand_r(seed)/(RAND_MAX+1.0),rand_r(seed)/(RAND_MAX+1.0),rand_r(seed)/(RAND_MAX+1.0)};
}

static Vec3 vec3_random_interval(interval Int, unsigned int *seed) {
    return vec3_add( (Vec3) {Int.min,Int.min,Int.min} ,vec3_scale(vec3_random(seed),(Int.max -Int.min )));
}

static Vec3 vec3_random_unit_vector(unsigned int *seed) {
    for (;;) {
        const Vec3 a = vec3_random_interval((interval) {-1,1}, seed);
        const double a_dot = vec3_length_sq(a);
        if (1e-160 < a_dot && a_dot <= 1) {
            return vec3_normalize(a);
        }
    }
}

//Material-helper functions
static Vec3 vec3_random_on_hemisphere(const Vec3 normal, unsigned int *seed) {
    const Vec3 on_unit_sphere = vec3_random_unit_vector(seed);
    if (vec3_dot(on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    }
    return vec3_scale(on_unit_sphere, -1);
}

static Vec3 vec3_random_unit_disk_vector(unsigned int *seed) {
    for (;;) {
        const Vec3 a = vec3(rand_r(seed) / (RAND_MAX + 1.0) * 2.0 - 1.0,rand_r(seed) / (RAND_MAX + 1.0) * 2.0 - 1.0,0.0);
        if (vec3_length_sq(a) <1) {
            return a;
        }
    }
}

static Vec3 vec3_reflect(const Vec3 v, const Vec3 normal) {
    return vec3_subtract(v, vec3_scale(normal, 2 * vec3_dot(v,normal)));
}

static Vec3 vec3_refract(const Vec3 v, const Vec3 normal, double etai_over_etat) {
    const double cos_theta = fmin(vec3_dot(vec3_negate(v),normal), 1.0);
    const Vec3 r_out_perp = vec3_scale(vec3_add(v, vec3_scale(normal, cos_theta)),etai_over_etat);
    const Vec3 r_out_para = vec3_scale(normal, -sqrt(fabs(1.0 - vec3_length_sq(r_out_perp))));
    return vec3_add(r_out_para, r_out_perp);
}

//Near-Zero functionality

static bool vec3_near_zero(const Vec3 v) {
    const double s = 1e-8;
    return (fabs(v.x) < s && fabs(v.y)<s && fabs(v.z)<s);
}




#endif