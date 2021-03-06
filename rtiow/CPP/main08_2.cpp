#include <iostream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#define RANDOM_IMPL
#include "random.h"

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
};

vec3 color(const ray& r, hitable *world) {
    hit_record rec;
    // remove acne by starting at 0.001
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        vec3 target = rec.normal + random_in_unit_sphere();
        return 0.5 * color(ray(rec.p, target), world);
    }
    else {
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
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = ((float)i + random_f()) / (float)nx;
                float v = ((float)j + random_f()) / (float)ny;
                ray r = cam.get_ray(u, v);
                col += color(r, world);
            }
            col /= (float)ns;
            col = vec3( sqrtf(col[0]), sqrtf(col[1]), sqrtf(col[2]) );
            int ir = (int)(255.99f*col[0]);
            int ig = (int)(255.99f*col[1]);
            int ib = (int)(255.99f*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
