#ifndef RANDOMH
#define RANDOMH

//#include <cstdlib>
#include <inttypes.h>
#include <limits.h>

#include "vec3.h"

#ifdef DEBUG
extern unsigned long rfcnt;
extern unsigned long riuscnt;
extern unsigned long riudcnt;
#define INLINE
#else
#define INLINE static inline
#endif

/**********************************************
 PCG random implementation - credits to Cieric
 */
typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;
#ifdef RANDOM_IMPL
static pcg32_random_t seed = {0, 0};
#endif

INLINE void pcg_srand(unsigned val) {
	seed.state = val;
	seed.inc = 0;
}

INLINE uint32_t pcg_rand() {
	uint64_t oldstate = seed.state;
	seed.state = oldstate * 6364136223846793005ULL + (seed.inc|1);
	uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
	uint32_t rot = oldstate >> 59u;
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
#define PCG_RAND_MAX UINT_MAX
/*
 **********************************************/

#ifdef RANDOM_IMPL
unsigned long rfcnt;
unsigned long riuscnt;
unsigned long riudcnt;
#endif



INLINE float random_f() {
#ifdef DEBUG
	rfcnt++;
#endif
//        return (float)pcg_rand() / ((float)PCG_RAND_MAX + (float)1.0);
        float r = (float)pcg_rand() / ((float)PCG_RAND_MAX + (float)1.0);
//        printf("r=%.6f\n", r);
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
