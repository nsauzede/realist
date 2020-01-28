#ifndef SPHEREH
#define SPHEREH

#include <math.h>

#include "hitable.h"

class sphere: public hitable  {
    public:
        sphere() {}
        sphere(vec3 cen, float r, material *m = 0)
            : center(cen), radius(r), mat_ptr(m)  {};
        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
        vec3 center;
        float radius;
        material *mat_ptr; /* NEW */
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
//    r.print();
    vec3 oc = r.origin() - center;
//    oc.print();
//    r.direction().print();
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
//    printf("a=%f b=%f c=%f d=%f \n", a, b, c, discriminant); // OK : identical
#if 0
    union {
        float f[4];
        uint32_t u[4];
    } u;
    u.f[0] = a; u.f[1] = b; u.f[2] = c; u.f[3] = discriminant;
    printf("a=%" PRIx32 " b=%" PRIx32 " c=%" PRIx32 " d=%" PRIx32 " \n",
       u.u[0], u.u[1], u.u[2], u.u[3]); // OK : identical
#endif
//    exit(0);
    if (discriminant > 0) {
        float temp = (-b - sqrtf(discriminant))/a;
//        printf("t1=%f\n", (double)temp);
        if (temp < t_max && temp > t_min) {
//            printf("t1=%f\n", (double)temp);
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr; /* NEW */
            return true;
        }
        temp = (-b + sqrtf(discriminant)) / a;
//        printf("t2=%f\n", (double)temp);
        if (temp < t_max && temp > t_min) {
//            printf("t2=%f\n", (double)temp);
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr; /* NEW */
            return true;
        }
    }
    return false;
}

#endif

