#include "sphere.h"
#include "math.h"

bool sphere_hit(const Surface *self, const Ray r, interval t, surface_record *rec) {
    const Sphere *s = (const Sphere *) self;
    const Vec3 oc = vec3_subtract(s->center, r.origin);
    const double a = vec3_length_sq(r.direction);
    const double b = vec3_dot(r.direction,oc);
    const double c = vec3_length_sq(oc) - s->radius*s->radius;
    double discrim = b*b - a*c;
    if (discrim < 0) {
        return false;
    }

    double root = (b - sqrt(discrim))/a;
    if (!interval_surrounds(t, root)) {
        root = (b + sqrt(discrim))/a;
        if (!interval_surrounds(t, root)){
            return false;
        }
    }

    rec->t = root;
    rec->p = ray_at(r,root);
    Vec3 outward_normal = vec3_scale(vec3_add(ray_at(r,root),s->center),1/s->radius);
    hit_record_set_face_normal(rec, r, outward_normal);

    return true;
}

Sphere sphere_create(Vec3 center, double radius) {
    Sphere s;
    s.base.hit = sphere_hit;
    s.center = center;
    s.radius = radius;
    return s;
}