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

    const double R = cos(M_PI/4);

    Lambertian left_mat = lambertian_create(color(0,0,1));
    Lambertian right_mat = lambertian_create(color(1,0,0));

    Sphere s1 = sphere_create(vec3(R, 0,    -1.0), R,   (Material *)&right_mat);
    Sphere s2 = sphere_create(vec3(-R, 0,    -1.0), R,   (Material *)&left_mat);

    surface_list_add(&world, (Surface *)&s1);
    surface_list_add(&world, (Surface *)&s2);

    const Camera cam = camera_create();

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
