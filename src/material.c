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

static bool metal_scatter(const Material *self, Ray r_in, const surface_record *rec, Color *attenuation, Ray *scattered, unsigned int *seed) {
    const Metal *met = (const Metal *)self;

    Vec3 reflected = vec3_reflect(r_in.direction, rec->normal);
    reflected = vec3_add(vec3_normalize(reflected), vec3_scale(vec3_random_unit_vector(seed), met->fuzz));
    *scattered = ray(rec->p, reflected);
    *attenuation = met->albedo;
    return vec3_dot(reflected, rec->normal) > 0;
}

Metal metal_create(const Color albedo, const double fuzz) {
    Metal met;
    met.base.scatter = metal_scatter;
    met.albedo = albedo;
    met.fuzz = fuzz;
    return met;
}

static double reflectance(const double cosine, const double refraction_index) {
    const double r0 = (1-refraction_index)/ (1+refraction_index);
    return (r0*r0) + ((1-r0) * pow((1-cosine), 5) );
}
static bool dielectric_scatter(const Material *self, Ray r_in, const surface_record *rec, Color *attenuation, Ray *scattered, unsigned int *seed) {
    const Dielectric *die = (const Dielectric *)self;

    double ri = rec->front_face ? (1.0/die->refraction_index): die->refraction_index;
    const Vec3 unit_direction = vec3_normalize(r_in.direction);
    const double cos_theta = fmin(vec3_dot(vec3_negate(unit_direction), rec->normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;

    Vec3 direction;

    if (cannot_refract || reflectance(cos_theta,ri)> rand_r(seed)/ (RAND_MAX + 1.0)) {
        direction = vec3_reflect(unit_direction, rec->normal);
    }
    else {
        direction = vec3_refract(unit_direction, rec->normal, ri);
    }
    *scattered = ray(rec->p, direction);
    *attenuation = die->albedo;
    return true;
}

Dielectric dielectric_create(const Color albedo, const double refraction_index) {
    Dielectric die;
    die.base.scatter = dielectric_scatter;
    die.albedo = albedo;
    die.refraction_index = refraction_index;
    return die;
}

