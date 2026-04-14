#include "material.h"
#include "surface_record.h"
#include "vec3.h"

static bool lambertian_scatter(const Material *self, Ray r_in, const surface_record *rec,
                                Color *attenuation, Ray *scattered, unsigned int *seed) {
    const Lambertian *lam = (const Lambertian *)self;

    Vec3 scatter_dir = vec3_add(rec->normal, vec3_random_unit_vector(seed));

    if (vec3_near_zero(scatter_dir)) {
        scatter_dir = rec->normal;
    }
    *scattered  = ray(rec->p, scatter_dir);
    *attenuation = lam->albedo;
    return true;
}

Lambertian lambertian_create(const Color albedo) {
    Lambertian lam;
    lam.base.scatter = lambertian_scatter;
    lam.albedo = albedo;
    return lam;
}
