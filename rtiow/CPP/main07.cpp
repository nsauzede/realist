#include <iostream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "random.h"
#if 1
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
#else
#include "camera.h"
#endif

vec3 color(const ray& r, hitable *world) {
    hit_record rec;
    if (world->hit(r, 0.0, FLT_MAX, rec)) {
//        printf("HIT!");
//        rec.normal.print();
//        return vec3(0, 0, 0);
        return 0.5*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);
    }
    else {
//        printf("NOT HIT!");
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

//extern "C" {
#if 0
#define API static inline
#else
#define API
#endif
API void vdiv(vec3& l, const vec3& r1, float r2) {
	l.e[0] = r1.e[0] / r2;
	l.e[1] = r1.e[1] / r2;
	l.e[2] = r1.e[2] / r2;
}
//}

int main(int argc, char *argv[]) {
    int nx = 200;
    int ny = 100;
    int ns = 100;
    int arg = 1;
    if (arg < argc) {
        sscanf(argv[arg++], "%d", &nx);
                if (arg < argc) {
                        sscanf(argv[arg++], "%d", &ny);
                        if (arg < argc) {
                                sscanf(argv[arg++], "%d", &ns);
                        }
                }
    }
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
//                printf("r1=%f r2=%f u=%f v=%f\n", r1, r2, u, v);
                ray r = cam.get_ray(u, v);
//                r.print(); // OK : identical
//                printf("  j=%d i=%d\n", j, i);
                //exit(0);
//                col += color(r, world);
                vec3 col0 = color(r, world);
//                col0.print();
                col += col0;
//                printf("  u=%f v=%f\n", u, v); // OK : identical
            }
//            col.print();
//            printf("\n");
            col /= (float)ns;
//            vdiv(col, col, (float)ns);
//            col.print();
//            printf("  j=%d i=%d\n", j, i);
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
