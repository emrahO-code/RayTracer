#include <stdio.h>
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "surface_list.h"
#include "surface.h"
#include "sphere.h"
#include <math.h>


Color ray_color(const Ray r, const Surface *world) {
    surface_record rec;
    if (world->hit(world,r,0,INFINITY, &rec)) {
        return color_scale(color_add(vec3_to_color(rec.normal), color(1,1,1)),0.5 );
    }

    Vec3 unit_direction = vec3_normalize(r.direction);
    double a = 0.5 * (unit_direction.y +1.0);
    Color white = color(1, 1, 1);
    Color blue = color(0.5, 0.7, 1.0);
    return color_lerp(white, blue, a);
}
int main() {

    double aspect_ratio = 16.0/9.0;
    int image_width = 400;
    int image_height = (int)(image_width/aspect_ratio);

    Surface_list world = surface_list_create();
    Sphere s1 = sphere_create(vec3(0,0,-1),0.5);
    Sphere s2 = sphere_create(vec3(0,-100.5,-1),100);

    surface_list_add(&world, (Surface *)&s1);
    surface_list_add(&world, (Surface *)&s2);

    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double)image_width / image_height;
    Vec3 camera_center = vec3(0,0,0);

    Vec3 viewport_u = vec3(viewport_width,0,0);
    Vec3 viewport_v = vec3(0, -viewport_height,0);

    Vec3 pixel_delta_u = vec3_scale(viewport_u, 1.0/image_width);
    Vec3 pixel_delta_v = vec3_scale(viewport_v, 1.0/image_height);

    Vec3 viewport_upper_left_1 = vec3_add(camera_center, vec3_negate(vec3(0,0,focal_length)));
    Vec3 viewport_upper_left_2 = vec3_add(viewport_upper_left_1, vec3_negate(vec3_scale(viewport_u, 1.0 / 2)));
    Vec3 viewport_upper_left = vec3_add(viewport_upper_left_2, vec3_negate(vec3_scale(viewport_v, 1.0 / 2)));

    Vec3 pixel00_loc = vec3_add(viewport_upper_left, vec3_scale(vec3_add(pixel_delta_u,pixel_delta_v),1.0/2));

    FILE *file = fopen("image.ppm", "w");

    fprintf(file, "P3\n%d %d\n255\n", image_width, image_height);

    for (int i = 0; i< image_height; i++) {
        for (int j = 0; j<image_width; j++) {
            Vec3 pixel_center = vec3_add(pixel00_loc, vec3_add(vec3_scale(pixel_delta_u,j), vec3_scale(pixel_delta_v,i)));
            Vec3 ray_direction = vec3_subtract(pixel_center,camera_center);
            Ray r = ray(camera_center,ray_direction);

            Color c = ray_color(r, (Surface *)&world);
            write_color(file, c);
        }
    }

    fclose(file);
    surface_list_free(&world);
    return 0;
}
