#ifndef VEC3_H
#define VEC3_H

#include <math.h>

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

inline Vec3 vec3_normalize(const Vec3 a) {
    return vec3_scale(a, 1.0/vec3_length(a));
}

#endif VEC3_H