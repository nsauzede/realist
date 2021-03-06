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
	void print() {
		printf("{\n");
		printf("\tlower_left_corner: ");
		lower_left_corner.print();
		printf(" \n");
		printf("\thorizontal: ");
		horizontal.print();
		printf(" \n");
		printf("\tvertical: ");
		vertical.print();
		printf(" \n");
		printf("\torigin: ");
		origin.print();
		printf(" \n");
		printf("}\n");
	}

        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
#ifdef DEBUG
printf("HIT\n");
#endif
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
#ifdef DEBUG
printf("ATT\n");
#endif
            return attenuation*color(scattered, world, depth+1);
        }
        else {
#ifdef DEBUG
printf("NOT ATT\n");
#endif
            return vec3(0,0,0);
        }
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

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

class metal : public material {
    public:
        metal(const vec3& a, float f) : albedo(a) {
            if (f < 1) fuzz = f; else fuzz = 1;
        }

        virtual bool scatter(const ray& r_in, const hit_record& rec,
            vec3& attenuation, ray& scattered) const
        {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
        vec3 albedo;
        float fuzz;
};
class lambertian : public material {
    public:
        lambertian(const vec3& a) : albedo(a) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec,
            vec3& attenuation, ray& scattered) const
        {
             vec3 target = rec.normal + random_in_unit_sphere();
             scattered = ray(rec.p, target);
             attenuation = albedo;
             return true;
        }

        vec3 albedo;
};
int main() {
    pcg_srand(0);
    int nx = 200;
    int ny = 100;
    int ns = 100;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    hitable *list[4];
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 1));
    list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.3));
    hitable *world = new hitable_list(list,4);
    camera cam;
#ifdef DEBUG
    cam.print();
    world->print();
#endif
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
#ifdef DEBUG
printf("rfcnt=%lu riuscnt=%lu riudcnt=%lu\n", rfcnt, riuscnt, riudcnt);
#endif
                float u = ((float)i + random_f()) / (float)nx;
                float v = ((float)j + random_f()) / (float)ny;
                ray r = cam.get_ray(u, v);
#ifdef DEBUG
                printf("j=%d i=%d r=", j, i);r.print();printf(" \n");
#endif
                col += color(r, world,0);
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
