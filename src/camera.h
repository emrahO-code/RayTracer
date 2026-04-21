#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H
#include "surface.h"
#include "ray.h"
#include "color.h"
#include "vec3.h"
#include <pthread.h>

#define TILE_SIZE 32
#define NUM_THREADS 8

typedef struct {
    int    image_width;
    double aspect_ratio;
    int    samples_per_pixel;
    int    max_depth;
    double vfov;
} CameraConfig;

typedef struct  {
    double aspect_ratio;
    int image_width;
    int samples_per_pixel;
    int sqrt_spp;
    double recip_sqrt_spp;
    int max_depth;
    double vfov;

    Vec3 camera_center;
    int image_height;
    Vec3 pixel00_loc;
    Vec3 pixel_delta_u;
    Vec3 pixel_delta_v;
    double pixel_samples_scale;
}Camera;

typedef struct {
    const Camera *cam;
    const Surface *world;
    Color *buffer;
    _Atomic int *next_tile;
    int tiles_x;
    int tiles_y;
} ThreadArgs;

Camera _camera_create(CameraConfig cfg);

#define camera_create(...) _camera_create((CameraConfig){   \
.image_width       = 400,                               \
.aspect_ratio      = 16.0 / 9.0,                       \
.samples_per_pixel = 100,                               \
.max_depth         = 50,                                \
.vfov              = 90.0,                              \
__VA_ARGS__                                             \
})


void camera_render(const Camera *cam, const Surface *world, FILE *file);


#endif //RAYTRACER_CAMERA_H