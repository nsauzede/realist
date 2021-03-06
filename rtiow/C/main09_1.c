#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <float.h>

#define RANDOM_IMPL
#include "random.h"
#include "vec3.h"
#include "ray.h"

struct hit_record_s;
struct material_s;

typedef struct hit_record_s {
	float t;
	vec3 p;
	vec3 normal;
	struct material_s *mat_ptr;
} hit_record;

typedef bool (*scatter_t)(struct material_s *p, const ray *r_in, const struct hit_record_s *rec, vec3 attenuation, ray *scattered);
typedef void (*print_t)(void *p);

typedef struct {
	vec3 albedo;
} lambertian_t;

typedef struct {
	vec3 albedo;
} metal_t;

typedef struct material_s {
	scatter_t scatter;
	print_t print;
	union {
		void *null;
		lambertian_t lambertian;
		metal_t metal;
	} u;
} material_t;

typedef struct {
	vec3 center;
	float radius;
	material_t mat;
} sphere_t;

#define MLAMBERTIAN(ax, ay, az) ((material_t){lambertian_scatter, lambertian_print, .u.lambertian=(lambertian_t){{ax, ay, az}}})
#define MMETAL(ax, ay, az) ((material_t){metal_scatter, metal_print, .u.metal=(metal_t){{ax, ay, az}}})

#define HSPHERE(cx, cy, cz, r, m) ((hittable_t){sphere_hit, sphere_print, .u.sphere=(sphere_t){{cx, cy, cz}, r, m}})
#define HSTART {list_hit, list_print, .u.null=0}
#define HEND {0, 0, .u.null=0}

struct hittable_s;
typedef bool (*hit_t)(struct hittable_s *p, const ray *r, float t_min, float t_max, hit_record *rec);

typedef struct hittable_s {
	hit_t hit;
	print_t print;
	union {
		void *null;
		sphere_t sphere;
	} u;
} hittable_t;

void list_print(void *_p) {
        hittable_t *p = (hittable_t *)_p;
        printf("[\n");
        while (1) {
                p++;
                if (!p->print) break;
                p->print(p);
                printf(",\n");
        }
        printf("]\n");
}

void sphere_print(void *_p) {
        hittable_t *p = (hittable_t *)_p;
        sphere_t *s = &p->u.sphere;
        printf("{HS:");vprint(s->center);printf(" ,%.6f,", s->radius);
//        p->mat.print(&p->mat);
        p->u.sphere.mat.print(&p->u.sphere.mat);
        printf("}");
}

void lambertian_print(void *_p) {
        material_t *p = (material_t *)_p;
        lambertian_t *l = &p->u.lambertian;
        printf("{ML:");vprint(l->albedo);printf(" }");
}

void metal_print(void *_p) {
        material_t *p = (material_t *)_p;
        metal_t *m = &p->u.metal;
        printf("{MM:");vprint(m->albedo);
        printf(" ");
//        printf(",%.6f", m->fuzz);
        printf("}");
}

bool list_hit(hittable_t *p, const ray *r, float t_min, float t_max, hit_record *rec) {
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

bool sphere_hit(hittable_t *p, const ray *r, float t_min, float t_max, hit_record *rec) {
	sphere_t *s = &p->u.sphere;
	vec3 oc;
	vsub(oc, r->origin, s->center);
	float a = vdot(r->direction, r->direction);
	float b = vdot(oc, r->direction);
	float c = vdot(oc, oc) - s->radius*s->radius;
//	vec3 abc = {a, b, c};
//	printf("abc=");vprint(abc);printf("\n");
	float discriminant = b*b - a*c;
	if (discriminant > 0) {
		float temp = (-b - sqrtf(discriminant))/a;
		if (temp < t_max && temp > t_min) {
			rec->t = temp;
			point_at_parameter(rec->p, r, rec->t);
			vsub(rec->normal, rec->p, s->center);
			vdiv(rec->normal, rec->normal, s->radius);
			rec->mat_ptr = &s->mat;
			return true;
		}
		temp = (-b + sqrtf(discriminant))/a;
		if (temp < t_max && temp > t_min) {
			rec->t = temp;
			point_at_parameter(rec->p, r, rec->t);
			vsub(rec->normal, rec->p, s->center);
			vdiv(rec->normal, rec->normal, s->radius);
			rec->mat_ptr = &s->mat;
			return true;
		}
	}
	return false;
}

bool lambertian_scatter(struct material_s *p, const ray *r_in, const hit_record *rec, vec3 attenuation, ray *scattered) {
	lambertian_t *l = &p->u.lambertian;
	vec3 target;
	random_in_unit_sphere(target);
	vadd(target, rec->normal, target);
	rmake(scattered, rec->p, target);
	vcopy(attenuation, l->albedo);
	return true;
}

void reflect(vec3 l, const vec3 v, const vec3 n) {
	vmul(l, 2.f * vdot(v, n), n);
	vsub(l, v, l);
}

bool metal_scatter(struct material_s *p, const ray *r_in, const hit_record *rec, vec3 attenuation, ray *scattered) {
	metal_t *m = &p->u.metal;
	vec3 uvector, reflected;
	unit_vector(uvector, r_in->direction);
	reflect(reflected, uvector, rec->normal);
	rmake(scattered, rec->p, reflected);
	vcopy(attenuation, m->albedo);
	return vdot(scattered->direction, rec->normal) > 0;
}

void color(vec3 col, const ray *r, hittable_t *world, int depth) {
	hit_record rec;
#ifdef DEBUG
	rprint(r);printf("\n");
#endif
	// remove acne by starting at 0.001
	if (world->hit(world, r, 0.001, FLT_MAX, &rec)) {
#ifdef DEBUG
		printf("HIT\n");
		printf("t=%.6f\n", (double)rec.t);
		printf("mat=");rec.mat_ptr->print(rec.mat_ptr);printf("\n");
#endif
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(rec.mat_ptr, r, &rec, attenuation, &scattered)) {
#ifdef DEBUG
			printf("ATT\n");
			vec3 tv = {rec.t, 0, 0};
			printf("tv=");
			vprint(tv);
			printf(" \np=");
			vprint(rec.p);
			printf(" \n");
			printf("nor=");
			vprint(rec.normal);
//			printf(" \n");
//			printf("h=");
//			h->print(h);
			printf("\nsca=");
			rprint(&scattered);
			printf(" \n");
#endif
			vec3 scat_col;
			color(scat_col, &scattered, world, depth + 1);
			vmulv(col, attenuation, scat_col);
		} else {
#ifdef DEBUG
			printf("NOT ATT\n");
#endif
			vcopy(col, VEC3(0, 0, 0));
		}
	} else {
		vec3 unit_direction;
		unit_vector(unit_direction, r->direction);
#ifdef DEBUG
		printf("NOT HIT");
		printf(" dir=");
		vprint(r->direction);
		printf(" ud=");
		vprint(unit_direction);
		printf(" \n");
#endif
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

void cam_print(const camera *cam) {
        printf("{\n\tlower_left_corner: ");vprint(cam->lower_left_corner);printf(" ");
        printf("\n\thorizontal: ");vprint(cam->horizontal);printf(" ");
        printf("\n\tvertical: ");vprint(cam->vertical);printf(" ");
        printf("\n\torigin: ");vprint(cam->origin);printf(" ");
        printf("\n}\n");
//        printf("\nu: ");vprint(cam->u);
//        printf("\nv: ");vprint(cam->v);
//        printf("\nw: ");vprint(cam->w);
//        printf("\nlens_radius=%.6f\n", cam->lens_radius);
}

void wprint(hittable_t *world) {
        if (!world->print) return;
        world->print(world);
}

int main() {
	pcg_srand(0);
	int nx = 200;
	int ny = 100;
	int ns = 100;
	printf("P3\n"); printf("%d %d\n", nx, ny); printf("255\n");
	hittable_t world[] = {
		HSTART,
		HSPHERE(0, 0, -1, 0.5, MLAMBERTIAN(0.8, 0.3, 0.3)),
		HSPHERE(0, -100.5, -1, 100, MLAMBERTIAN(0.8, 0.8, 0)),
		HSPHERE(1, 0, -1, 0.5, MMETAL(0.8, 0.6, 0.2)),
		HSPHERE(-1, 0, -1, 0.5, MMETAL(0.8, 0.8, 0.8)),
		HEND
	};
	camera cam = {
		.lower_left_corner = {-2.0, -1.0, -1.0},
		.horizontal = {4.0, 0.0, 0.0},
		.vertical = {0.0, 2.0, 0.0},
		.origin = {0.0, 0.0, 0.0}
	};
#ifdef DEBUG
	cam_print(&cam);
	wprint(world);
#endif
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col = {0, 0, 0};
			for (int s = 0; s < ns; s++) {
				float u = ((float)i + random_f()) / (float)nx;
				float v = ((float)j + random_f()) / (float)ny;
				ray r;
				get_ray(&cam, &r, u, v);
#ifdef DEBUG
//				printf("j=%d i=%d s=%d r=", j, i, s);
				printf("r=");
				rprint(&r);
				printf("\n");
#endif
				vec3 col0;
				color(col0, &r, world, 0);
				vadd(col, col, col0);
#ifdef DEBUG
				printf("col=");vprint(col);printf("\n");
#endif
			}
			vdiv(col, col, (float)ns);
			// Gamma 2 correction (square root)
			col[0] = sqrtf(col[0]);
			col[1] = sqrtf(col[1]);
			col[2] = sqrtf(col[2]);

			int ir = (int)(255.99f*col[0]);
			int ig = (int)(255.99f*col[1]);
			int ib = (int)(255.99f*col[2]);
			printf("%d %d %d\n", ir, ig, ib);
		}
	}
	return 0;
}
