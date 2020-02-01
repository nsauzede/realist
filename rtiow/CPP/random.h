    #ifndef RANDOMH
    #define RANDOMH

    #include <cstdlib>

#include "vec3.h"

#ifdef DEBUG
extern unsigned long rfcnt;
#define INLINE
#else
#define INLINE inline
#endif

INLINE float random_f() {
#ifdef DEBUG
	rfcnt++;
#endif
        return (float)rand() / ((float)RAND_MAX + (float)1.0);
}
//extern unsigned long riuscnt;
vec3 random_in_unit_sphere() {
    vec3 p;
#ifdef DEBUG
    riuscnt++;
#endif
    do {
        float r1 = random_f();
        float r2 = random_f();
        float r3 = random_f();
        p = 2.0*vec3(r1, r2, r3) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}
    #endif

