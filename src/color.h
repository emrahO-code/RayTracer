#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>

typedef struct {
    double x,y,z;
}Color;

//Basic Constructor and Basic Scalar Multiplication and Vector Addition
static Color color(const double x, const double y,const double z) {
    return (Color){x,y,z};
}

static Color color_add(const Color a, const Color b) {
    return (Color){a.x+b.x,a.y+b.y,a.z+b.z};
}

static Color color_subtract(const Color a, const Color b) {
    return (Color){a.x-b.x, a.y-b.y, a.z-b.z};
}

static Color color_scale(const Color a, const double c) {
    return (Color){c * a.x,c * a.y , c * a.z};
}

static Color color_negate(const Color c) {
    return (Color){-c.x, -c.y, -c.z};
}

static Color color_lerp(const Color a, const Color b, const double c) {
    return color_add(color_scale(a,1.0-c), color_scale(b,c));
}

static void write_color(FILE* f, const Color c) {
    double r = c.x < 0 ? 0 : c.x > 1 ? 1 : c.x;
    double g = c.y < 0 ? 0 : c.y > 1 ? 1 : c.y;
    double b = c.z < 0 ? 0 : c.z > 1 ? 1 : c.z;
    int ir = (int) (255.999 * r);
    int ig = (int) (255.999 * g);
    int ib = (int) (255.999 * b);

    fprintf(f, "%d %d %d \n", ir,ig,ib);
}

static Color vec3_to_color(Vec3 v) {
    return (Color){v.x,v.y,v.z};
}
#endif //COLOR_H