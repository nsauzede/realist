#ifndef VEC3_H
#define VEC3_H

#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

// #define USE_DOUBLE

#ifdef USE_DOUBLE
typedef double scalar;
#define ssqrt sqrt
#define sabs fabs
#define spow pow
#else
typedef float scalar;
#define ssqrt sqrtf
#define sabs fabsf
#define spow powf
#endif

typedef scalar vec3[3];
#define VEC3(x, y, z) ((vec3){x, y, z})

#define API static inline
// #define API

static inline scalar smin(scalar x, scalar y) {
  if (x < y)
    return x;
  else
    return y;
}

static inline void vprint(const vec3 r) {
#if 0
	printf("{%.6f, %.6f, %.6f}", r[0], r[1], r[2]);
//	printf("{%g, %g, %g}", r[0], r[1], r[2]);
#else
#ifdef USE_DOUBLE
  union {
    double v[3];
    uint64_t i[3];
  } u;
  u.v[0] = r[0];
  u.v[1] = r[1];
  u.v[2] = r[2];
  printf("{%.6f, %.6f, %.6f;%" PRIx64 ", %" PRIx64 ", %" PRIx64 "}", r[0], r[1],
         r[2], u.i[0], u.i[1], u.i[2]);
#else
  union {
    float v[3];
    uint32_t i[3];
  } u;
  u.v[0] = r[0];
  u.v[1] = r[1];
  u.v[2] = r[2];
  //	printf("{0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 "}", u.i[0], u.i[1],
  // u.i[2]);
  printf("{%.6f, %.6f, %.6f;%" PRIx32 ", %" PRIx32 ", %" PRIx32 "}", r[0], r[1],
         r[2], u.i[0], u.i[1], u.i[2]);
#endif
#endif
}

API void vcopy(vec3 l, const vec3 r) {
  l[0] = r[0];
  l[1] = r[1];
  l[2] = r[2];
}

API scalar vsqlen(const vec3 r) {
  return r[0] * r[0] + r[1] * r[1] + r[2] * r[2];
}

API scalar vlen(const vec3 r) { return ssqrt(vsqlen(r)); }

API void vadd(vec3 l, const vec3 r1, const vec3 r2) {
  l[0] = r1[0] + r2[0];
  l[1] = r1[1] + r2[1];
  l[2] = r1[2] + r2[2];
}

API void vsub(vec3 l, const vec3 r1, const vec3 r2) {
  l[0] = r1[0] - r2[0];
  l[1] = r1[1] - r2[1];
  l[2] = r1[2] - r2[2];
}

API void vmul(vec3 l, scalar r1, const vec3 r2) {
  l[0] = r1 * r2[0];
  l[1] = r1 * r2[1];
  l[2] = r1 * r2[2];
}

API void vmulv(vec3 l, const vec3 r1, const vec3 r2) {
  l[0] = r1[0] * r2[0];
  l[1] = r1[1] * r2[1];
  l[2] = r1[2] * r2[2];
}

API void vcross(vec3 l, const vec3 r1, const vec3 r2) {
  l[0] = r1[1] * r2[2] - r1[2] * r2[1];
  l[1] = r1[2] * r2[0] - r1[0] * r2[2];
  l[2] = r1[0] * r2[1] - r1[1] * r2[0];
}

API void vdiv(vec3 l, const vec3 r1, scalar r2) {
  l[0] = r1[0] / r2;
  l[1] = r1[1] / r2;
  l[2] = r1[2] / r2;
}

API scalar vdot(const vec3 r1, const vec3 r2) {
  return r1[0] * r2[0] + r1[1] * r2[1] + r1[2] * r2[2];
}

API void unit_vector(vec3 l, const vec3 r) { vdiv(l, r, vlen(r)); }

API int near_zero(const vec3 r) {
  const scalar s = 1e-8;
  return sabs(r[0]) < s && sabs(r[1]) < s && sabs(r[2]) < s;
}

#endif /*VEC3_H*/
