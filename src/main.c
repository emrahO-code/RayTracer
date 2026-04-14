#include <stdio.h>
#include "vec3.h"
#include "surface_list.h"
#include "surface.h"
#include "sphere.h"
#include <math.h>
#include "camera.h"
#include "material.h"

int main() {

    Surface_list world = surface_list_create();

    Lambertian mat1 = lambertian_create(color(1, 0, 0));
    Lambertian mat2 = lambertian_create(color(0.3, 1, 0.3));

    Sphere s1 = sphere_create(vec3(0, 0,    -1), 0.5,   (Material *)&mat1);
    Sphere s2 = sphere_create(vec3(0, -100.5, -1), 100, (Material *)&mat2);

    surface_list_add(&world, (Surface *)&s1);
    surface_list_add(&world, (Surface *)&s2);

    const Camera cam = camera_create(1000,16.0/9.0,100,10);

    FILE *file = fopen("image.ppm", "w");
    if (!file) {
        fprintf(stderr, "Failed to open file\n");
        return 1;
    }

    camera_render(&cam,(Surface *)&world, file);

    fclose(file);
    surface_list_free(&world);
    return 0;
}
