#include "camera.h"
#include <math.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include "surface_record.h"
#include "material.h"

#define FOCAL_LENGTH 1
#define VIEWPORT_HEIGHT 2


Color ray_color(const Ray r, const Surface *world, const int depth, unsigned int *seed) {
    if (depth <=0 ) {
        return (Color){0,0,0};
    }
    surface_record rec;
    if (world->hit(world,r,(interval){0.001,INFINITY}, &rec)) {
        Ray scattered;
        Color attenuation;
        if (rec.mat->scatter(rec.mat, r, &rec, &attenuation, &scattered, seed)) {
            const Color bounced = ray_color(scattered, world, depth - 1, seed);
            // Modulate: attenuation * bounced (component-wise multiply)
            return color(attenuation.x * bounced.x,
                         attenuation.y * bounced.y,
                         attenuation.z * bounced.z);
        }
        return color(0, 0, 0);
    }

    const Vec3 unit = vec3_normalize(r.direction);
    const double a = 0.5 * (unit.y + 1.0);
    return color_lerp(color(1, 1, 1), color(0.1, 0.5, 1.0), a);
}

Ray get_ray(const Camera *cam,int i, int j, int si, int sj, unsigned int *seed) {
    const Vec3 offset = {
        (sj + rand_r(seed) / (RAND_MAX + 1.0)) * cam->recip_sqrt_spp - 0.5,
        (si + rand_r(seed) / (RAND_MAX + 1.0)) * cam->recip_sqrt_spp - 0.5,
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

void *render_tiles(void *arg) {
    ThreadArgs *a = (ThreadArgs *)arg;
    const Camera *cam = a->cam;
    const int total_tiles = a->tiles_x * a->tiles_y;

    unsigned int seed = arc4random();

    int tile_idx;
    while ((tile_idx = atomic_fetch_add(a->next_tile,1)) < total_tiles) {
        const int tile_x = tile_idx % a->tiles_x;
        const int tile_y = tile_idx / a->tiles_x;

        const int x_start = tile_x * TILE_SIZE;
        const int y_start = tile_y * TILE_SIZE;
        const int x_end = x_start + TILE_SIZE < cam->image_width  ? x_start + TILE_SIZE : cam->image_width;
        const int y_end = y_start + TILE_SIZE < cam->image_height ? y_start + TILE_SIZE : cam->image_height;

        for (int i = y_start; i<y_end; i++) {
            for (int j = x_start; j<x_end; j++) {
                Color pixel_color = color(0,0,0);

                for (int si = 0; si < cam->sqrt_spp; si++) {
                    for (int sj = 0; sj < cam->sqrt_spp; sj++) {
                        const Ray r = get_ray(cam, j, i, si, sj, &seed);
                        pixel_color = color_add(pixel_color, ray_color(r, a->world, cam->max_depth, &seed));
                    }
                }

                a->buffer[i * cam->image_width + j] = color_scale(pixel_color, cam->pixel_samples_scale);
            }
        }
    }
    return 0;
}

void camera_render(const Camera *cam, const Surface *world, FILE *file) {
    Color *buffer = malloc(cam->image_height * cam->image_width *sizeof(Color));

    const int tiles_x = (cam->image_width  + TILE_SIZE - 1) / TILE_SIZE;
    const int tiles_y = (cam->image_height + TILE_SIZE - 1) / TILE_SIZE;

    _Atomic int next_tile = 0;

    ThreadArgs args[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    for (int i =0; i < NUM_THREADS; i++) {
        args[i] = (ThreadArgs){
            .cam = cam,
            .world = world,
            .buffer = buffer,
            .tiles_x = tiles_x,
            .tiles_y = tiles_y,
            .next_tile = &next_tile,
        };
        pthread_create(&threads[i], 0, render_tiles, &args[i]);
    }

    for (int i = 0; i< NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    fprintf(file, "P3\n%d %d\n255\n", cam->image_width, cam->image_height);

    for (int i= 0; i<cam->image_height * cam->image_width; i++) {
        write_color(file, buffer[i]);
    }

    free(buffer);
}