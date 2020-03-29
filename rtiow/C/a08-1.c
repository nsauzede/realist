#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <float.h>

#include "vec3.h"
#include "ray.h"

#ifdef DEBUG
unsigned long rfcnt = 0;
unsigned long riudcnt = 0;
unsigned long riuscnt = 0;
#endif

static inline float random_f() {
#ifdef DEBUG
	rfcnt++;
#endif
    float r = (float)rand() / (RAND_MAX + 1.0);
#ifdef DEBUG
	printf("r=%f ", r);
#endif
    return r;
}

void random_in_unit_sphere(vec3 p) {
#ifdef DEBUG
	riuscnt++;
#endif
	do {
		float r1 = random_f();
		float r2 = random_f();
		float r3 = random_f();
		vmul(p, 2.0, VEC3(r1,r2,r3));
		vsub(p, p, VEC3(1,1,1));
#ifdef DEBUG
		printf("p=");vprint(p);printf(" \n");
#endif
	} while (vsqlen(p) >= 1.0);
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
//	printf("max=%f\n", t_max);
	hit_record temp_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;
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
//	float c = vdot(oc, oc) - s->radius*s->radius;
	float ra = s->radius*s->radius;
	float c = vdot(oc, oc) - ra;
	float discriminant = b*b - a*c;
#ifdef DEBUG
//	printf("ray=");rprint(r);printf(" \noc=");vprint(oc);printf(" \na=%f b=%f ra=%f c=%f d=%f\n", a, b, ra, c, discriminant);
	printf("sc=");vprint(s->center);printf(" \n");
	printf("ray=");rprint(r);printf(" \n");
	printf("oc=");vprint(oc);printf(" \n");
	printf("a=%f\n", a);
	printf("b=%f\n", b);
	printf("ra=%f\n", ra);
	printf("c=%f\n", c);
	printf("d=%f\n", discriminant);
#endif
	if (discriminant > 0) {
#ifdef DEBUG
		float temp1 = (-b - sqrtf(discriminant))/a;
		float temp2 = (-b + sqrtf(discriminant))/a;
		vec3 t1t2 = {temp1, temp2, 0};
		printf("t1t2=");vprint(t1t2);printf(" \n");
		if (temp1 < t_max && temp1 > t_min) {
			rec->t = temp1;
#else
		float temp = (-b - sqrtf(discriminant))/a;
		if (temp < t_max && temp > t_min) {
			rec->t = temp;
#endif
			point_at_parameter(rec->p, r, rec->t);
			vsub(rec->normal, rec->p, s->center);
			vdiv(rec->normal, rec->normal, s->radius);
			return true;
		}
#ifdef DEBUG
		if (temp2 < t_max && temp2 > t_min) {
			rec->t = temp2;
#else
		temp = (-b + sqrtf(discriminant))/a;
		if (temp < t_max && temp > t_min) {
			rec->t = temp;
#endif
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
	float max = FLT_MAX;
//	printf("max=%f\n", max);
//	if (world->hit(world, r, 0, FLT_MAX, &rec)) {
	if (world->hit(world, r, 0.001, FLT_MAX, &rec)) {
#ifdef DEBUG
printf("HIT\n");
		printf("nrm=");vprint(rec.normal);printf(" \n");
#endif
		ray ray0;
		vec3 target;
		random_in_unit_sphere(target);
		vadd(target, rec.normal, target);
		rmake(&ray0, rec.p, target);
		color(col, &ray0, world);
		vmul(col, 0.5, col);
	} else {
#ifdef DEBUG
printf("NOT HIT\n");
#endif
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
#ifdef DEBUG
	vec3 uv={u, v, 0};
	printf("uv=");vprint(uv);printf(" \n");
#endif
	vec3 direction, direction0, direction1;
	vmul(direction0, v, cam->vertical);
	vmul(direction1, u, cam->horizontal);
	vadd(direction, direction0, direction1);
	vadd(direction, direction, cam->lower_left_corner);
	vsub(direction, direction, cam->origin);
	rmake(r, cam->origin, direction);
#ifdef DEBUG
	printf("r=");rprint(r);printf(" \n");
#endif
}

int main() {
	srand(0);
	int nx = 200;
	int ny = 100;
	int ns = 100;
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
#ifdef DEBUG
	printf("{\n");
	printf("\tlower_left_corner: ");vprint(cam.lower_left_corner);printf(" \n");
	printf("\thorizontal: ");vprint(cam.horizontal);printf(" \n");
	printf("\tvertical: ");vprint(cam.vertical);printf(" \n");
	printf("\torigin: ");vprint(cam.origin);printf(" \n");
	printf("}\n");
#endif
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col = {0, 0, 0};
			for (int s = 0; s < ns; s++) {
#ifdef DEBUG
//printf("rfcnt=%lu riuscnt=%lu riudcnt=%lu\n", rfcnt, riuscnt, riudcnt);
#endif
				float u = ((float)i + (float)random_f()) / (float)nx;
				float v = ((float)j + (float)random_f()) / (float)ny;
#ifdef DEBUG
//                printf("u=%f v=%f\n", u, v);
#endif
//				vec3 uv={u, v, 0};
//				printf("uv=");vprint(uv);printf(" \n");
				ray r;
				get_ray(&cam, &r, u, v);
#ifdef DEBUG
                printf("r=");rprint(&r);printf(" \n");
#endif
				vec3 col0;
				color(col0, &r, world);
				vadd(col, col, col0);
#ifdef DEBUG
printf("rfcnt=%lu riuscnt=%lu riudcnt=%lu\n", rfcnt, riuscnt, riudcnt);
#endif
			}
			vdiv(col, col, (float)ns);
			int ir = (int)(255.99f*col[0]);
			int ig = (int)(255.99f*col[1]);
			int ib = (int)(255.99f*col[2]);
			printf("%d %d %d\n", ir, ig, ib);
		}
	}
	return 0;
}
