#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H
#include "surface.h"
#include "ray.h"
#include "color.h"
#include "vec3.h"

typedef struct Camera {
    double aspect_ratio;
    int image_width;
    int samples_per_pixel;
    int sqrt_spp;
    double recip_sqrt_spp;

    Vec3 camera_center;
    int image_height;
    Vec3 pixel00_loc;
    Vec3 pixel_delta_u;
    Vec3 pixel_delta_v;
    double pixel_samples_scale;
}Camera;

Camera camera_create(int image_width, double aspect_ratio, int samples_per_pixel);
void camera_render(const Camera *cam, const Surface *world, FILE *file);


#endif //RAYTRACER_CAMERA_H