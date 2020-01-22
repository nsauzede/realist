#ifndef RAYH
#define RAYH

#include <stdio.h>

#include "vec3.h"

class ray
{
    public:
        ray() {}
        ray(const vec3& a, const vec3& b) { A = a; B = b; }
        vec3 origin() const       { return A; }
        vec3 direction() const    { return B; }
//        vec3 point_at_parameter(float t) const { return A + t*B; }
        vec3 point_at_parameter(float t) const { return A + t * B; }
	void print() const {
#if 0
		printf("{%f, %f, %f}, {%f, %f, %f}}",
			A.e[0], A.e[1], A.e[2],
			B.e[0], B.e[1], B.e[2]
		);
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

