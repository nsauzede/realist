#include <iostream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#define RANDOM_IMPL
#include "random.h"

#ifdef DEBUG
unsigned long rfcnt = 0;
unsigned long riudcnt = 0;
unsigned long riuscnt = 0;
#endif

class camera {
    public:
        camera() {
            lower_left_corner = vec3(-2.0, -1.0, -1.0);
            horizontal = vec3(4.0, 0.0, 0.0);
            vertical = vec3(0.0, 2.0, 0.0);
            origin = vec3(0.0, 0.0, 0.0);
        }
        ray get_ray(float u, float v) {
            return ray(origin,
                       lower_left_corner + u*horizontal + v*vertical - origin);
        }

        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
	void print() {
		printf("Origin: ");origin.print();
		printf("\nLower_left: ");lower_left_corner.print();
		printf("\nhorizontal: ");horizontal.print();
		printf("\nvertical: ");vertical.print();
#if 0
		printf("\nu: ");u.print();
		printf("\nv: ");v.print();
		printf("\nw: ");w.print();
		printf("\nlens_radius=%.6f\n", lens_radius);
#endif
	}
};

vec3 color(const ray& r, hitable *world) {
    hit_record rec;
//    if (world->hit(r, 0, FLT_MAX, rec)) {
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
#ifdef DEBUG
    	printf("HIT\n");
#endif
        vec3 target = rec.normal + random_in_unit_sphere();
        return 0.5 * color(ray(rec.p, target), world);
    }
    else {
#ifdef DEBUG
    	printf("NOT HIT\n");
#endif
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

int main() {
    pcg_srand(0);
    int nx = 200;
    int ny = 100;
    int ns = 100;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    hitable *list[2];
    list[0] = new sphere(vec3(0,0,-1), 0.5);
    list[1] = new sphere(vec3(0,-100.5,-1), 100);
    hitable *world = new hitable_list(list,2);
    camera cam;
//    cam.print();
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
#ifdef DEBUG
printf("rfcnt=%lu riuscnt=%lu riudcnt=%lu\n", rfcnt, riuscnt, riudcnt);
#endif
                float u = ((float)i + random_f()) / (float)nx;
                float v = ((float)j + random_f()) / (float)ny;
#ifdef DEBUG
                printf("u=%.6f v=%.6f\n", u, v);
#endif
                ray r = cam.get_ray(u, v);
#ifdef DEBUG
                printf("r=");r.print();printf(" \n");
#endif
                col += color(r, world);
            }
            col /= (float)ns;
            int ir = (int)(255.99f*col[0]);
            int ig = (int)(255.99f*col[1]);
            int ib = (int)(255.99f*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
