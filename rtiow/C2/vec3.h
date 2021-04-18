#ifndef VEC3_H
#define VEC3_H

#include <inttypes.h>

#include <math.h>

typedef float vec3[3];
#define VEC3(x,y,z) ((vec3){x, y, z})

#define API static inline
// #define API

void vprint(const vec3 r) {
#if 0
	printf("{%.6f, %.6f, %.6f}", r[0], r[1], r[2]);
//	printf("{%g, %g, %g}", r[0], r[1], r[2]);
#else
#if 1
	union {
		float v[3];
		uint32_t i[3];
	} u;
	u.v[0] = r[0];
	u.v[1] = r[1];
	u.v[2] = r[2];
//	printf("{0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 "}", u.i[0], u.i[1], u.i[2]);
	printf("{%.6f, %.6f, %.6f;%" PRIx32 ", %" PRIx32 ", %" PRIx32 "}", r[0], r[1], r[2], u.i[0], u.i[1], u.i[2]);
#else
	printf("pointer : %d\n", (int)sizeof(void *));
	printf("float : %d\n", (int)sizeof(float));
	printf("double : %d\n", (int)sizeof(double));
	exit(0);
#endif
#endif
}

API void vcopy(vec3 l, const vec3 r) {
	l[0] = r[0];
	l[1] = r[1];
	l[2] = r[2];
}

API float vsqlen(const vec3 r) {
	return r[0] * r[0] + r[1] * r[1] + r[2] * r[2];
}

API float vlen(const vec3 r) {
	return sqrtf(vsqlen(r));
}

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

API void vmul(vec3 l, float r1, const vec3 r2) {
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

API void vdiv(vec3 l, const vec3 r1, float r2) {
	l[0] = r1[0] / r2;
	l[1] = r1[1] / r2;
	l[2] = r1[2] / r2;
}

API float vdot(const vec3 r1, const vec3 r2) {
	return r1[0] * r2[0]
		+ r1[1] * r2[1]
		+ r1[2] * r2[2];
}

API void unit_vector(vec3 l, const vec3 r) {
	vdiv(l, r, vlen(r));
}

#endif/*VEC3_H*/
