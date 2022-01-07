#ifndef RANDOMH
#define RANDOMH

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
void pcg_srand(unsigned val) {
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
#else
extern void pcg_srand(unsigned val) {
extern uint32_t pcg_rand() {
#endif

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
        return (float)pcg_rand() / ((float)PCG_RAND_MAX + (float)1.0);
}
uint32_t n_rand = 0;
// unroll loop tentative by Cieric
// seems slightly slower..
//#define UNROLL_LOOP
#ifdef UNROLL_LOOP
void random_in_unit_sphere(vec3 p) {
#ifdef DEBUG
    riuscnt++;
#endif
    float u = random_f();
    float v = random_f();
    float theta = u * 2.0f * (float)M_PI;
    float phi = acosf(2.0f * v - 1.0f);
    float r = cbrtf(random_f());
    float sinTheta = sinf(theta);
    float cosTheta = cosf(theta);
    float sinPhi = sinf(phi);
    float cosPhi = cosf(phi);
    p[0] = r * sinPhi * cosTheta;
    p[1] = r * sinPhi * sinTheta;
    p[2] = r * cosPhi;
}

void random_in_unit_disk(vec3 p) {
#ifdef DEBUG
    riudcnt++;
#endif
    float a = random_f() * 2.0f * (float)M_PI;
    float r = sqrt(random_f());
    p[0] = r * cosf(a);
    p[1] = r * sinf(a);
    p[2] = 0.0f;
}
#else
void random_in_unit_sphere(vec3 p) {
#ifdef DEBUG
        riuscnt++;
#endif
        do {
                float r1 = random_f();
                float r2 = random_f();
                float r3 = random_f();
                n_rand+=3;
                vmul(p, 2.0, VEC3(r1,r2,r3));
                vsub(p, p, VEC3(1,1,1));
        } while (vsqlen(p) >= 1.0);
}

void random_in_unit_disk(vec3 p) {
#ifdef DEBUG
        riudcnt++;
#endif
        do {
                float r1 = random_f();
                float r2 = random_f();
                n_rand+=2;
                vmul(p, 2.0, VEC3(r1,r2,0));
                vsub(p, p, VEC3(1,1,0));
        } while (vsqlen(p) >= 1.0);
}
#endif

#endif
