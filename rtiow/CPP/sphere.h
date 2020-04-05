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
        virtual void print() const;
        vec3 center;
        float radius;
        material *mat_ptr; /* NEW */
};

void sphere::print() const {
	printf("{S:");center.print();printf(" ,%.6f}", radius);
}

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrtf(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr; /* NEW */
            return true;
        }
        temp = (-b + sqrtf(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
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

