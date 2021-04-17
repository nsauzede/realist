#ifndef RAYH
#define RAYH

#include <inttypes.h>

#include <stdio.h>

#include "vec3.h"

class ray
{
    public:
        ray() {}
        ray(const vec3& a, const vec3& b) { A = a; B = b; }
        inline vec3 origin() const       { return A; }
        inline vec3 direction() const    { return B; }
//        vec3 point_at_parameter(float t) const { return A + t*B; }
        inline vec3 point_at_parameter(float t) const { return A + t * B; }
	void print() const {
#if 0
		union {
			float v[3];
			uint32_t i[3];
		} u;
		printf("{");
		u.v[0] = A.e[0];
		u.v[1] = A.e[1];
		u.v[2] = A.e[2];
		printf("{%" PRIx64 ", %" PRIx64 ", %" PRIx64 "}", u.i[0], u.i[1], u.i[2]);
		printf(", ");
		u.v[0] = B.e[0];
		u.v[1] = B.e[1];
		u.v[2] = B.e[2];
		printf("{%" PRIx64 ", %" PRIx64 ", %" PRIx64 "}", u.i[0], u.i[1], u.i[2]);
		printf("}");
#else
		printf("{");
		A.print();
		printf(", ");
		B.print();
		printf("}");
#endif
	}

        vec3 A;
        vec3 B;
};

#endif

