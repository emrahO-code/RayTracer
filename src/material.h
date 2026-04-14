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


#endif