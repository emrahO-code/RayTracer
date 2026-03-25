#include "camera.h"
#include <math.h>
#include <stdio.h>

#define FOCAL_LENGTH 1
#define VIEWPORT_HEIGHT 2

Color ray_color(const Ray r, const Surface *world) {
    surface_record rec;
    if (world->hit(world,r,(interval){0,INFINITY}, &rec)) {
        return color_scale(color_add(vec3_to_color(rec.normal), color(1,1,1)),0.5 );
    }

    const Vec3 unit_direction = vec3_normalize(r.direction);
    const double a = 0.5 * (unit_direction.y +1.0);
    const Color white = color(1, 1, 1);
    const Color blue = color(0.5, 0.7, 1.0);
    return color_lerp(white, blue, a);
}

Camera camera_create(const int image_width, const double aspect_ratio) {
    Camera cam;
    cam.image_width = image_width;
    cam.aspect_ratio = aspect_ratio;
    cam.camera_center = vec3(0,0,0);

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

    const Vec3 origin    = cam->camera_center;
    const Vec3 p00       = cam->pixel00_loc;
    const Vec3 delta_u   = cam->pixel_delta_u;
    const Vec3 delta_v   = cam->pixel_delta_v;
    const int  width     = cam->image_width;
    const int  height    = cam->image_height;

    for (int i = 0; i< height; i++) {
        for (int j = 0; j<width; j++) {
            const Vec3 pixel_center = vec3_add(p00, vec3_add(vec3_scale(delta_u,j), vec3_scale(delta_v,i)));
            const Vec3 ray_direction = vec3_subtract(pixel_center,origin);
            const Ray r = ray(origin,ray_direction);

            const Color c = ray_color(r, world);
            write_color(file, c);
        }
    }
}