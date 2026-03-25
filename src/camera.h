#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H
#include "surface.h"
#include "ray.h"
#include "color.h"
#include "vec3.h"

typedef struct Camera {
    double aspect_ratio;
    int image_width;
    Vec3 camera_center;
    int image_height;
    Vec3 pixel00_loc;
    Vec3 pixel_delta_u;
    Vec3 pixel_delta_v;
}Camera;

Camera camera_create(int image_width, double aspect_ratio);
void camera_render(const Camera *cam, const Surface *world, FILE *file);

#endif //RAYTRACER_CAMERA_H