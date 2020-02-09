    #ifndef RANDOMH
    #define RANDOMH

    #include <cstdlib>

#include "vec3.h"

#ifdef DEBUG
extern unsigned long rfcnt;
extern unsigned long riuscnt;
extern unsigned long riudcnt;
#define INLINE
#else
#define INLINE inline
#endif

#ifdef RANDOM_IMPL
unsigned long rfcnt;
unsigned long riuscnt;
unsigned long riudcnt;
#endif

INLINE float random_f() {
#ifdef DEBUG
	rfcnt++;
#endif
//        return (float)rand() / ((float)RAND_MAX + (float)1.0);
        float r = (float)rand() / ((float)RAND_MAX + (float)1.0);
//        printf("r=%f\n", r);
        return r;
}
vec3 random_in_unit_sphere() {
#ifdef DEBUG
    riuscnt++;
#endif
    vec3 p;
    do {
        float r1 = random_f();
        float r2 = random_f();
        float r3 = random_f();
        p = 2.0*vec3(r1, r2, r3) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}

vec3 random_in_unit_disk() {
#ifdef DEBUG
    riudcnt++;
#endif
    vec3 p;
    do {
        float r1 = random_f();
        float r2 = random_f();
        p = 2.0*vec3(r1,r2,0) - vec3(1,1,0);
//    } while (dot(p,p) >= 1.0);
    } while (p.squared_length() >= 1.0);
    return p;
}
    #endif

