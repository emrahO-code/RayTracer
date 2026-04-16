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

    Lambertian ground_mat = lambertian_create(color(.8, .8, 0));
    Lambertian center_mat = lambertian_create(color(0.1, 0.2, 0.5));
    Dielectric left_mat = dielectric_create(color(1,.6,.7),1.50);
    Dielectric bubble_mat = dielectric_create(color(1,1,1),1.0/1.5);
    Metal mat3 = metal_create(color(.8,.6,.2), .0);

    Sphere ground = sphere_create(vec3(0, -100.5, -1.0), 100, (Material *)&ground_mat);
    Sphere s1 = sphere_create(vec3(0, 0,    -1.2), 0.5,   (Material *)&center_mat);
    Sphere s2 = sphere_create(vec3(-1.0, 0,    -1.0), 0.5,   (Material *)&left_mat);
    Sphere bubble  = sphere_create(vec3(-1.0, 0,    -1.0), 0.4,   (Material *)&bubble_mat);
    Sphere s3 = sphere_create(vec3(1.0, 0,    -1.0), 0.5,   (Material *)&mat3);

    surface_list_add(&world, (Surface *)&s1);
    surface_list_add(&world, (Surface *)&ground);
    surface_list_add(&world, (Surface *)&s2);
    surface_list_add(&world, (Surface *)&s3);
    surface_list_add(&world, (Surface *)&bubble);

    const Camera cam = camera_create(1000,16.0/9.0,500,10);

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
