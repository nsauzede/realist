#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <float.h>

#include "vec3.h"
#include "ray.h"

inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

typedef struct hit_record_s {
	float t;
	vec3 p;
	vec3 normal;
} hit_record;

typedef struct {
	vec3 center;
	float radius;
} sphere_t;

struct hittable_s;
typedef bool (*hit_t)(struct hittable_s *p, const ray *r, float t_min, float t_max, hit_record *rec);

typedef struct hittable_s {
	hit_t hit;
	union {
		void *null;
		sphere_t sphere;
	} obj;
} hittable;

bool list_hit(hittable *p, const ray *r, float t_min, float t_max, hit_record *rec) {
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;
	while (1) {
		p++;
		if (!p->hit) break;
		if (p->hit(p, r, t_min, closest_so_far, &temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			*rec = temp_rec;
		}
	}
	return hit_anything;
}

bool sphere_hit(hittable *p, const ray *r, float t_min, float t_max, hit_record *rec) {
	sphere_t *s = &p->obj.sphere;
	vec3 oc;
	vsub(oc, r->origin, s->center);
	float a = vdot(r->direction, r->direction);
	float b = vdot(oc, r->direction);
	float c = vdot(oc, oc) - s->radius*s->radius;
	float discriminant = b*b - a*c;
//	printf("a=%f b=%f c=%f d=%f \n", a, b, c, discriminant); // OK : identical
#if 0
	    union {
        float f[4];
        uint32_t u[4];
    } u;
    u.f[0] = a;
    u.f[1] = b;
    u.f[2] = c;
    u.f[3] = discriminant;
    printf("a=%" PRIx32 " b=%" PRIx32 " c=%" PRIx32 " d=%" PRIx32 " \n", u.u[0], u.u[1], u.u[2], u.u[3]); // OK : identical
#endif
	if (discriminant > 0) {
		float temp = (-b - sqrtf(discriminant))/a;
//		printf("t1=%f\n", (double)temp);
		if (temp < t_max && temp > t_min) {
//			printf("t1=%f\n", (double)temp);
			rec->t = temp;
			point_at_parameter(rec->p, r, rec->t);
			vsub(rec->normal, rec->p, s->center);
			vdiv(rec->normal, rec->normal, s->radius);
			return true;
		}
		temp = (-b + sqrtf(discriminant))/a;
//		printf("t2=%f\n", (double)temp);
		if (temp < t_max && temp > t_min) {
//			printf("t2=%f\n", (double)temp);
			rec->t = temp;
			point_at_parameter(rec->p, r, rec->t);
			vsub(rec->normal, rec->p, s->center);
			vdiv(rec->normal, rec->normal, s->radius);
			return true;
		}
	}
	return false;
}

#define HSPHERE(cx, cy, cz, r) ((hittable){sphere_hit, .obj.sphere=(sphere_t){{cx, cy, cz}, r}})
#define HSTART ((hittable){list_hit, 0})
#define HEND ((hittable){0, 0})

void color(vec3 col, const ray *r, hittable *world) {
	if (!world->hit) return;
	hit_record rec;
	if (world->hit(world, r, 0.0, FLT_MAX, &rec)) {
//		printf("HIT!");
//		vprint(rec.normal);
		vadd(col, rec.normal, VEC3(1, 1, 1));
		vmul(col, 0.5, col);
//		vcopy(col, VEC3(0, 0, 0));
	} else {
//		printf("NOT HIT!");
		vec3 unit_direction;
		unit_vector(unit_direction, r->direction);
		float t = 0.5*(unit_direction[1] + 1.0);
		vec3 col0 = {1.0, 1.0, 1.0};
		vec3 col1 = {0.5, 0.7, 1.0};
		vmul(col0, 1.0-t, col0);
		vmul(col1, t, col1);
		vadd(col, col0, col1);
	}
}

typedef struct {
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
} camera;

void get_ray(camera *cam, ray *r, float u, float v) {
	vec3 direction, direction0, direction1;
	vmul(direction0, v, cam->vertical);
	vmul(direction1, u, cam->horizontal);
	vadd(direction, direction0, direction1);
	vadd(direction, direction, cam->lower_left_corner);
	vsub(direction, direction, cam->origin);
	rmake(r, cam->origin, direction);
}

int main(int argc, char *argv[]) {
	srand(0);
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
	printf("P3\n"); printf("%d %d\n", nx, ny); printf("255\n");
	hittable world[] = {
		HSTART,
		HSPHERE(0, 0, -1, 0.5),
		HSPHERE(0, -100.5, -1, 100),
		HEND
	};
	camera cam = {
		.lower_left_corner = {-2.0, -1.0, -1.0},
		.horizontal = {4.0, 0.0, 0.0},
		.vertical = {0.0, 2.0, 0.0},
		.origin = {0.0, 0.0, 0.0}
	};
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col = {0, 0, 0};
			for (int s = 0; s < ns; s++) {
//				float u = (float)(i + random_double()) / (float)nx;
//				float v = (float)(j + random_double()) / (float)ny;
				double r1 = random_double();
				double r2 = random_double();
				double u = (i + r1) / nx;
				double v = (j + r2) / ny;
//				printf("r1=%f r2=%f u=%f v=%f\n", r1, r2, u, v);
				ray r;
				get_ray(&cam, &r, u, v);
//				rprint(&r); // OK : identical
				vec3 col0;
				color(col0, &r, world);
//				vprint(col0);
				vadd(col, col, col0);
//				printf("  u=%f v=%f\n", u, v); // OK : identical
			}
//			vprint(col);
//			printf("\n");
			vdiv(col, col, (float)ns);
//			vprint(col);
//			printf("  j=%d i=%d\n", j, i);
			int ir = (int)(255.99*col[0]);
			int ig = (int)(255.99*col[1]);
			int ib = (int)(255.99*col[2]);
			printf("%d %d %d\n", ir, ig, ib);
		}
	}
}
