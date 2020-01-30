    #ifndef RANDOMH
    #define RANDOMH

    #include <cstdlib>

#include "vec3.h"

inline float random_f() {
        return (float)rand() / ((float)RAND_MAX + (float)1.0);
}
vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        float r1 = random_f();
        float r2 = random_f();
        float r3 = random_f();
        p = 2.0*vec3(r1, r2, r3) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}
    #endif

