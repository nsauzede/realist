#ifndef RAYH
#define RAYH

#include "vec3.h"

typedef struct ray_s {
#if 0
        ray() {}
        ray(const vec3& a, const vec3& b) { A = a; B = b; }
        vec3 origin() const       { return A; }
        vec3 direction() const    { return B; }
        vec3 point_at_parameter(float t) const { return A + t*B; }
#endif
	vec3 origin;	// A
	vec3 direction;	// B
} ray;

void rprint(const ray *r) {
	printf("{");
	vprint(r->origin);
	printf(", ");
	vprint(r->direction);
	printf("}");
}

API void rmake(ray *l, const vec3 r1, const vec3 r2) {
	vcopy(l->origin, r1);
	vcopy(l->direction, r2);
}

API void point_at_parameter(vec3 l, const ray *r1, float r2) {
	vmul(l, r2, r1->direction);
	vadd(l, r1->origin, l);
}

#endif
