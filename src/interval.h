#ifndef RAYTRACER_INTERVAL_H
#define RAYTRACER_INTERVAL_H

typedef struct interval {
    double min;
    double max;
}interval;

static double interval_size(const interval t) {
    return t.max-t.min;
}

static bool interval_contains(const interval t,const double x) {
    return t.min <= x && x <= t.max;
}
static bool interval_surrounds(const interval t,const double x) {
    return t.min < x && x < t.max;
}

static double interval_clamp(const interval t,const double x) {
    if (x<t.min) {
       return t.min;
    }
    if (x>t.max) {
        return t.max;
    }
    return x;
}




#endif //RAYTRACER_INTERVAL_H