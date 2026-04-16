#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include "color.h"
#include "ray.h"
#include "surface_record.h"

typedef struct Material Material;

struct Material {
    bool (*scatter)(const Material *self, Ray r_in, const surface_record *rec, Color *attenuation, Ray *scattered, unsigned int *seed);
};

typedef struct {
    Material base;
    Color albedo;
} Lambertian;

Lambertian lambertian_create(Color albedo);

typedef struct {
    Material base;
    Color albedo;
    double fuzz;
} Metal;

Metal metal_create(Color albedo, double fuzz);

typedef struct {
    Material base;
    double refraction_index;
    Color albedo;
} Dielectric;

Dielectric dielectric_create(Color albedo, double refraction_index);


#endif