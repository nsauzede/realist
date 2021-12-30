#ifndef RANDOMH
#define RANDOMH

#include <cstdlib>

#include "vec3.h"

    inline double random_double() {
        return rand() / (RAND_MAX + 1.0);
    }
    inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(random_double(), random_double(), random_double()) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}

vec3 random_on_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(random_double(),random_double(),random_double()) - vec3(1,1,1);
    } while (dot(p,p) >= 1.0);
    return unit_vector(p);
}

#endif
