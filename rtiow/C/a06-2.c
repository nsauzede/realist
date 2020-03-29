#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <float.h>

#include "vec3.h"
#include "ray.h"

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
	float c = vdot(oc, oc) - s->radius*s->radius;
	float discriminant = b*b - a*c;
	if (discriminant > 0) {
		float temp = (-b - sqrtf(discriminant))/a;
		if (temp < t_max && temp > t_min) {
			rec->t = temp;
			point_at_parameter(rec->p, r, rec->t);
			vsub(rec->normal, rec->p, s->center);
			vdiv(rec->normal, rec->normal, s->radius);
			return true;
		}
		temp = (-b + sqrtf(discriminant))/a;
		if (temp < t_max && temp > t_min) {
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
		vadd(col, rec.normal, VEC3(1, 1, 1));
		vmul(col, 0.5, col);
	} else {
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

int main() {
	int nx = 200;
	int ny = 100;
	printf("P3\n"); printf("%d %d\n", nx, ny); printf("255\n");
	vec3 lower_left_corner = {-2.0, -1.0, -1.0};
	vec3 horizontal = {4.0, 0.0, 0.0};
	vec3 vertical = {0.0, 2.0, 0.0};
	vec3 origin = {0.0, 0.0, 0.0};
	hittable world[] = {
		HSTART,
		HSPHERE(0, 0, -1, 0.5),
		HSPHERE(0, -100.5, -1, 100),
		HEND
	};
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			float u = (float)i / (float)nx;
			float v = (float)j / (float)ny;
			ray r;
			vec3 direction, direction0, direction1;
			vmul(direction0, v, vertical);
			vmul(direction1, u, horizontal);
			vadd(direction, direction0, direction1);
			vadd(direction, direction, lower_left_corner);
			rmake(&r, origin, direction);
			vec3 col;
			color(col, &r, world);
			int ir = (int)(255.99f*col[0]);
			int ig = (int)(255.99f*col[1]);
			int ib = (int)(255.99f*col[2]);
			printf("%d %d %d\n", ir, ig, ib);
		}
	}
	return 0;
}
