#ifndef VEC3_H
#define VEC3_H

#include <math.h>

typedef float vec3[3];
#define VEC3(x,y,z) ((vec3){x, y, z})

#define API static inline

API void vprint(const vec3 r) {
	printf("{%f, %f, %f}", r[0], r[1], r[2]);
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
	return sqrt(vsqlen(r));
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
