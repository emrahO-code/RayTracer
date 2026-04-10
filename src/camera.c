#include "camera.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define FOCAL_LENGTH 1
#define VIEWPORT_HEIGHT 2


Color ray_color(const Ray r, const Surface *world, const int depth) {
    if (depth <=0 ) {
        return (Color){0,0,0};
    }
    surface_record rec;
    if (world->hit(world,r,(interval){0.001,INFINITY}, &rec)) {
        Vec3 direction = vec3_add(rec.normal, vec3_random_unit_vector());
        return color_scale(ray_color(ray(rec.p, direction), world, depth - 1), 0.5);
    }

    const Vec3 unit_direction = vec3_normalize(r.direction);
    const double a = 0.5 * (unit_direction.y +1.0);
    const Color white = color(1.0, 1.0, 1.0);
    const Color blue = color(0.1, 0.5, 1.0);
    return color_lerp(white, blue, a);
}

Ray get_ray(const Camera *cam,int i, int j, int si, int sj) {
    const Vec3 offset = {
        (sj + rand() / (RAND_MAX + 1.0)) * cam->recip_sqrt_spp - 0.5,
        (si + rand() / (RAND_MAX + 1.0)) * cam->recip_sqrt_spp - 0.5,
            0
    };
    Vec3 pixel_sample = vec3_add(
        cam->pixel00_loc,
        vec3_add(vec3_scale(cam->pixel_delta_u, i + offset.x),
        vec3_scale(cam->pixel_delta_v, j + offset.y)));

    Vec3 ray_direction = vec3_subtract(pixel_sample, cam->camera_center);
    return (Ray){cam->camera_center, ray_direction};
}

Camera camera_create(const int image_width, const double aspect_ratio, const int samples_per_pixel, const int max_depth) {
    Camera cam;
    cam.image_width = image_width;
    cam.aspect_ratio = aspect_ratio;
    cam.camera_center = vec3(0,0,0);
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;

    cam.sqrt_spp = (int)sqrt(samples_per_pixel);
    cam.recip_sqrt_spp = 1.0/cam.sqrt_spp;

    cam.pixel_samples_scale = 1.0 / samples_per_pixel;

    cam.image_height = (int)(image_width/aspect_ratio);
    const double viewport_width = VIEWPORT_HEIGHT * (double)image_width / cam.image_height;

    const Vec3 viewport_u = vec3(viewport_width,0,0);
    const Vec3 viewport_v = vec3(0, -VIEWPORT_HEIGHT,0);

    cam.pixel_delta_u = vec3_scale(viewport_u, 1.0/image_width);
    cam.pixel_delta_v = vec3_scale(viewport_v, 1.0/cam.image_height);

    const Vec3 viewport_upper_left = vec3_subtract(
        vec3_subtract(cam.camera_center, vec3(0, 0, FOCAL_LENGTH)),
        vec3_scale(vec3_add(viewport_u, viewport_v), 0.5)
    );

    cam.pixel00_loc = vec3_add(viewport_upper_left, vec3_scale(vec3_add(cam.pixel_delta_u,cam.pixel_delta_v),0.5));

    return cam;
}

void camera_render(const Camera *cam, const Surface *world, FILE *file) {
    fprintf(file, "P3\n%d %d\n255\n", cam->image_width, cam->image_height);

    for (int i = 0; i< cam->image_height; i++) {
        for (int j = 0; j<cam->image_width; j++) {
            Color pixel_color = color(0,0,0);

            for (int si = 0; si < cam->sqrt_spp; si++) {
                for (int sj = 0; sj < cam->sqrt_spp; sj++) {
                    const Ray r = get_ray(cam, j, i, si, sj);
                    pixel_color = color_add(pixel_color, ray_color(r, world, cam->max_depth));
                }
            }
            write_color(file, color_scale(pixel_color, cam->pixel_samples_scale));
        }
    }
}