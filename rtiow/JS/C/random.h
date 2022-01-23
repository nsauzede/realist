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
typedef struct {
  uint64_t state;
  uint64_t inc;
} pcg32_random_t;
#ifdef RANDOM_IMPL
static pcg32_random_t seed = {0, 0};
static inline void pcg_srand(unsigned val) {
  seed.state = val;
  seed.inc = 0;
}

INLINE uint32_t pcg_rand() {
  uint64_t oldstate = seed.state;
  seed.state = oldstate * 6364136223846793005ULL + (seed.inc | 1);
  uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  uint32_t rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
#else
extern void pcg_srand(unsigned val);
extern uint32_t pcg_rand();
#endif

#define PCG_RAND_MAX UINT_MAX
/*
 **********************************************/
#ifdef DEBUG
#ifdef RANDOM_IMPL
unsigned long rfcnt;
unsigned long riuscnt;
unsigned long riudcnt;
#endif
#endif

INLINE scalar random_s() {
#ifdef DEBUG
  rfcnt++;
#endif
  return (scalar)pcg_rand() / ((scalar)PCG_RAND_MAX + (scalar)1.0);
}
#ifdef DEBUG
uint32_t n_rand = 0;
#endif
static inline void random_in_unit_sphere(vec3 p) {
#ifdef DEBUG
  riuscnt++;
#endif
  do {
    scalar r1 = random_s();
    scalar r2 = random_s();
    scalar r3 = random_s();
#ifdef DEBUG
    n_rand += 3;
#endif
    vmul(p, 2.0, VEC3(r1, r2, r3));
    vsub(p, p, VEC3(1, 1, 1));
  } while (vsqlen(p) >= 1.0);
}

static inline void random_in_unit_disk(vec3 p) {
#ifdef DEBUG
  riudcnt++;
#endif
  do {
    scalar r1 = random_s();
    scalar r2 = random_s();
#ifdef DEBUG
    n_rand += 2;
#endif
    vmul(p, 2.0, VEC3(r1, r2, 0));
    vsub(p, p, VEC3(1, 1, 0));
  } while (vsqlen(p) >= 1.0);
}

#endif
