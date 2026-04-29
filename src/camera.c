#include "camera.h"
#include <math.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include "surface_record.h"
#include "material.h"
#include "png.h"


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

Vec3 defocus_disk_sample(const Camera *cam, unsigned int *seed) {
    const Vec3 a = vec3_random_unit_disk_vector(seed);
    return vec3_add(cam->camera_center, vec3_add(vec3_scale(cam->defocus_disk_u, a.x), vec3_scale(cam->defocus_disk_v, a.y)));

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
    Vec3 ray_origin = (cam->defocus_angle <=0) ? cam->camera_center : defocus_disk_sample(cam,seed);
    return (Ray){ray_origin, ray_direction};
}

Camera _camera_create(const CameraConfig cfg) {
    Camera cam;
    cam.image_width = cfg.image_width;
    cam.aspect_ratio = cfg.aspect_ratio;
    cam.camera_center = cfg.lookfrom;
    cam.samples_per_pixel = cfg.samples_per_pixel;
    cam.max_depth = cfg.max_depth;
    cam.vfov = cfg.vfov;
    cam.defocus_angle = cfg.defocus_angle;
    cam.focus_dist = cfg.focus_dist;

    cam.sqrt_spp = (int)sqrt(cfg.samples_per_pixel);
    cam.recip_sqrt_spp = 1.0/cam.sqrt_spp;

    cam.w = vec3_normalize(vec3_subtract(cfg.lookfrom, cfg.lookat));
    cam.u = vec3_normalize(vec3_cross(cfg.vup,cam.w));
    cam.v = vec3_cross(cam.w,cam.u);

    cam.pixel_samples_scale = 1.0 / cfg.samples_per_pixel;

    cam.image_height = (int)(cfg.image_width/cfg.aspect_ratio);
    const double theta  = M_PI * cfg.vfov/180;
    const double h = tan(theta/2);
    const double viewport_height = 2*h*cam.focus_dist;
    const double viewport_width = viewport_height * (double)cfg.image_width / cam.image_height;

    const Vec3 viewport_u = vec3_scale(cam.u,viewport_width);
    const Vec3 viewport_v = vec3_scale(cam.v, -viewport_height);

    cam.pixel_delta_u = vec3_scale(viewport_u, 1.0/cfg.image_width);
    cam.pixel_delta_v = vec3_scale(viewport_v, 1.0/cam.image_height);

    const Vec3 viewport_upper_left = vec3_subtract(
        vec3_subtract(cam.camera_center, vec3_scale(cam.w,cam.focus_dist)),
        vec3_scale(vec3_add(viewport_u, viewport_v), 0.5)
    );

    cam.pixel00_loc = vec3_add(viewport_upper_left, vec3_scale(vec3_add(cam.pixel_delta_u,cam.pixel_delta_v),0.5));
    double defocus_radius = cam.focus_dist * tan(M_PI*cam.defocus_angle/360);
    cam.defocus_disk_u = vec3_scale(cam.u,defocus_radius);
    cam.defocus_disk_v = vec3_scale(cam.v,defocus_radius);
    return cam;
}

void *render_tiles(void *arg) {
    const ThreadArgs *a = (ThreadArgs *)arg;
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

    write_png(file, buffer, cam->image_width, cam->image_height);

    free(buffer);
}