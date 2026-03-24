#include <stdio.h>
#include "vec3.h"
#include "color.h"
#include "ray.h"

Color ray_color(const Ray r) {
    Vec3 unit_direction = vec3_normalize(r.direction);
    double a = 0.5*(unit_direction.y + 1.0);
    Color white = color(1,1,1);
    Color blue = color(.5,.7,1);
    return color_lerp(white,blue,a);
}
int main() {

    double aspect_ratio = 16.0/9.0;
    int image_width = 400;
    int image_height = (int)(image_width/aspect_ratio);


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

            Color c = ray_color(r);
            write_color(file, c);
        }
    }
    fclose(file);
    return 0;
}
