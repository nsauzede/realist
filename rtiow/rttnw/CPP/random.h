#ifndef RANDOMH
#define RANDOMH

#include <cstdlib>

#include "vec3.h"

    inline double random_double() {
        return rand() / (RAND_MAX + 1.0);
    }
vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(random_double(), random_double(), random_double()) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}

#endif
