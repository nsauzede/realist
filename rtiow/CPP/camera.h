#ifndef CAMERAH
#define CAMERAH

#include "random.h"
#include "ray.h"

class camera {
public:
	camera() {
		lower_left_corner = vec3(-2.0, -1.0, -1.0);
		horizontal = vec3(4.0, 0.0, 0.0);
		vertical = vec3(0.0, 2.0, 0.0);
		origin = vec3(0.0, 0.0, 0.0);
	}
	camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
		float aperture = 0;
		float focus_dist = (lookfrom-lookat).length();
            lens_radius = aperture / 2;
            float theta = vfov*(float)M_PI/180;
            float half_height = tanf(theta/2);
            float half_width = aspect * half_height;
            origin = lookfrom;
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            lower_left_corner = origin
                              - half_width * focus_dist * u
                              - half_height * focus_dist * v
                              - focus_dist * w;
            horizontal = 2*half_width*focus_dist*u;
            vertical = 2*half_height*focus_dist*v;
	}
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect,
               float aperture, float focus_dist) {
            lens_radius = aperture / 2;
            float theta = vfov*(float)M_PI/180;
            float half_height = tanf(theta/2);
            float half_width = aspect * half_height;
            origin = lookfrom;
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            lower_left_corner = origin
                              - half_width * focus_dist * u
                              - half_height * focus_dist * v
                              - focus_dist * w;
            horizontal = 2*half_width*focus_dist*u;
            vertical = 2*half_height*focus_dist*v;
        }
	void print() {
		printf("Origin: ");origin.print();
		printf("\nLower_left: ");lower_left_corner.print();
		printf("\nhorizontal: ");horizontal.print();
		printf("\nvertical: ");vertical.print();
		printf("\nu: ");u.print();
		printf("\nv: ");v.print();
		printf("\nw: ");w.print();
		printf("\nlens_radius=%f\n", lens_radius);
	}

	ray get_ray(float s, float t) {
#ifdef DEBUG
		printf("s=%g t=%g\n", s, t);
#endif
#if 0
		vec3 rd = lens_radius*random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		return ray(origin + offset,
			lower_left_corner + s*horizontal + t*vertical
			- origin - offset);
#else
		vec3 rd = lens_radius*random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		ray r = ray(origin + offset,
			lower_left_corner + s*horizontal + t*vertical
			- origin - offset);
		return r;
#endif
	}

        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        float lens_radius;
};
#endif

