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

typedef bool (*scatter_t)(struct material_s *p, const ray *r_in, const struct hit_record_s *rec, vec3 attenuation, ray *scattered);

typedef struct hit_record_s {
	float t;
	vec3 p;
	vec3 normal;
	struct material_s *mat_ptr;
} hit_record;

typedef struct {
	vec3 albedo;
} lambertian_t;

typedef struct {
	vec3 albedo;
	float fuzz;
} metal_t;

typedef struct {
	float ref_idx;
} dielectric_t;

typedef struct material_s {
	scatter_t scatter;
	union {
		void *null;
		lambertian_t lambertian;
		metal_t metal;
		dielectric_t dielectric;
	} u;
} material_t;

typedef struct {
	vec3 center;
	float radius;
	material_t mat;
} sphere_t;

#define MLAMBERTIAN(ax, ay, az) ((material_t){lambertian_scatter, .u.lambertian=(lambertian_t){{ax, ay, az}}})
#define MMETAL(ax, ay, az, f) ((material_t){metal_scatter, .u.metal=(metal_t){{ax, ay, az}, f}})
#define MDIELECTRIC(r) ((material_t){dielectric_scatter, .u.dielectric=(dielectric_t){r}})

#define HSPHERE(cx, cy, cz, r, m) ((hittable_t){sphere_hit, sphere_print, .u.sphere=(sphere_t){{cx, cy, cz}, r, m}})
#define HSTART ((hittable_t){list_hit, list_print})
#define HEND ((hittable_t){0, 0})

struct hittable_s;
typedef bool (*hit_t)(struct hittable_s *p, const ray *r, float t_min, float t_max, hit_record *rec);
typedef void (*print_t)(struct hittable_s *p);

typedef struct hittable_s {
	hit_t hit;
	print_t print;
	union {
		void *null;
		sphere_t sphere;
	} u;
} hittable_t;

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

void list_print(hittable_t *p) {
	printf("[");
	int first = 1;
	while (1) {
		p++;
		if (!p->print) break;
		if (!first) {
			printf(", ");
		} else {
			first = 0;
		}
		p->print(p);
	}
	printf("]\n");
}

void wprint(hittable_t *world) {
	if (!world->print) return;
	world->print(world);
}


void sphere_print(hittable_t *p) {
	sphere_t *s = &p->u.sphere;
	printf("{S:");vprint(s->center);printf(" ,%.6f}", s->radius);
}

bool sphere_hit(hittable_t *p, const ray *r, float t_min, float t_max, hit_record *rec) {
	sphere_t *s = &p->u.sphere;
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

bool metal_scatter(struct material_s *p, const ray *r_in, const hit_record *rec,
		vec3 attenuation, ray *scattered) {
	metal_t *m = &p->u.metal;
	vec3 uvector, reflected, fuzz;
	unit_vector(uvector, r_in->direction);
	reflect(reflected, uvector, rec->normal);
	random_in_unit_sphere(fuzz);
	vmul(fuzz, m->fuzz, fuzz);
	vadd(reflected, reflected, fuzz);
	rmake(scattered, rec->p, reflected);
	vcopy(attenuation, m->albedo);
	return vdot(scattered->direction, rec->normal) > 0;
}

bool refract(const vec3 v, const vec3 n, float ni_over_nt, vec3 refracted) {
	vec3 uv;
	unit_vector(uv, v);
	float dt = vdot(uv, n);
	float discriminant = 1.0f - ni_over_nt*ni_over_nt*(1.-dt*dt);
	if (discriminant > 0) {
		vec3 v1, v2;
		vmul(v1, dt, n);
		vsub(v1, uv, v1);
		vmul(v1, ni_over_nt, v1);
		vmul(v2, sqrtf(discriminant), n);
		vsub(refracted, v1, v2);
		return true;
	}
	else
		return false;
}

float schlick(float cosine, float ref_idx) {
	float r0 = (1-ref_idx) / (1+ref_idx);
	r0 = r0*r0;
	return r0 + (1-r0)*powf((1 - cosine),5);
}

bool dielectric_scatter(struct material_s *p, const ray *r_in,
		const hit_record *rec, vec3 attenuation, ray *scattered) {
	dielectric_t *d = &p->u.dielectric;
	vec3 outward_normal;
	vec3 reflected;
	reflect(reflected, r_in->direction, rec->normal);
	float ni_over_nt;
	vcopy(attenuation, VEC3(1.0, 1.0, 1.0));
	vec3 refracted;
	float reflect_prob;
	float cosine;

	if (vdot(r_in->direction, rec->normal) > 0) {
		vmul(outward_normal, -1, rec->normal);
		ni_over_nt = d->ref_idx;
		cosine = d->ref_idx * vdot(r_in->direction, rec->normal)
			/ vlen(r_in->direction);
	} else {
		vcopy(outward_normal, rec->normal);
		ni_over_nt = 1.0f / d->ref_idx;
		cosine = -vdot(r_in->direction, rec->normal)
			/ vlen(r_in->direction);
	}

	if (refract(r_in->direction, outward_normal, ni_over_nt, refracted)) {
		reflect_prob = schlick(cosine, d->ref_idx);
	} else {
		reflect_prob = 1.0;
	}
	if (random_f() < reflect_prob) {
		rmake(scattered, rec->p, reflected);
	} else {
		rmake(scattered, rec->p, refracted);
	}

	return true;
}

void color(vec3 col, const ray *r, hittable_t *world, int depth) {
	hit_record rec;
	// remove acne by starting at 0.001
//	rprint(r);
	if (world->hit(world, r, 0.001, FLT_MAX, &rec)) {
//		printf("HIT\n");
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(rec.mat_ptr, r, &rec, attenuation, &scattered)) {
			vec3 scat_col;
			color(scat_col, &scattered, world, depth + 1);
			vmulv(col, attenuation, scat_col);
		} else {
			vcopy(col, VEC3(0, 0, 0));
		}
	} else {
//		printf("NOT HIT\n");
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

// vfov is top to bottom in degrees
void make_camera(camera *cam, const vec3 lookfrom, const vec3 lookat,
		const vec3 vup, float vfov, float aspect) {
//	vprint(lookfrom);printf("\n");
	vec3 u, v, w;
	float theta = vfov*(float)M_PI/180;
	float half_height = tanf(theta/2);
	float half_width = aspect * half_height;

	vcopy(cam->origin, lookfrom);
	vsub(w, lookfrom, lookat);
	unit_vector(w, w);
	vcross(u, vup, w);
	unit_vector(u, u);
	vcross(v, w, u);
	vec3 tmp;
	vmul(tmp, half_width, u);
	vsub(cam->lower_left_corner, cam->origin, tmp);
	vmul(tmp, half_height, v);
	vsub(cam->lower_left_corner, cam->lower_left_corner, tmp);
	vsub(cam->lower_left_corner, cam->lower_left_corner, w);
	vmul(cam->horizontal, 2 * half_width, u);
	vmul(cam->vertical, 2 * half_height, v);
#if 0
	vprint(cam->origin);printf("\n");
	vprint(cam->lower_left_corner);printf("\n");
	vprint(cam->horizontal);printf("\n");
	vprint(cam->vertical);printf("\n");
#endif
}

void cam_print(const camera *cam) {
	printf("{\n\tlower_left_corner: ");vprint(cam->lower_left_corner);printf(" ");
	printf("\n\thorizontal: ");vprint(cam->horizontal);printf(" ");
	printf("\n\tvertical: ");vprint(cam->vertical);printf(" ");
	printf("\n\torigin: ");vprint(cam->origin);printf(" ");
	printf("\n}\n");
//	printf("\nu: ");vprint(cam->u);
//	printf("\nv: ");vprint(cam->v);
//	printf("\nw: ");vprint(cam->w);
//	printf("\nlens_radius=%.6f\n", cam->lens_radius);
}

void get_ray(camera *cam, ray *r, float s, float t) {
//	printf("s=%.6f t=%.6f\n", s, t);
	vec3 direction, direction0, direction1;
	vmul(direction0, t, cam->vertical);
//	vprint(direction0);printf("\n");
	vmul(direction1, s, cam->horizontal);
//	vprint(direction1);printf("\n");
	vadd(direction, direction0, direction1);
//	vprint(direction);printf("\n");
	vadd(direction, direction, cam->lower_left_corner);
	vsub(direction, direction, cam->origin);
	rmake(r, cam->origin, direction);
//	rprint(r);printf("\n");
}

#ifdef DEBUG
unsigned long rfcnt = 0;
unsigned long riuscnt = 0;
unsigned long riudcnt = 0;
#endif

int main() {
	pcg_srand(0);
	int nx = 200;
	int ny = 100;
	int ns = 100;
	printf("P3\n"); printf("%d %d\n", nx, ny); printf("255\n");
	hittable_t world[] = {
		HSTART,
		HSPHERE(0, 0, -1, 0.5, MLAMBERTIAN(0.1, 0.2, 0.5)),
		HSPHERE(0, -100.5, -1, 100, MLAMBERTIAN(0.8, 0.8, 0)),
		HSPHERE(1, 0, -1, 0.5, MMETAL(0.8, 0.6, 0.2, 0.3)),
		HSPHERE(-1, 0, -1, 0.5, MDIELECTRIC(1.5)),
		HSPHERE(-1, 0, -1, -0.45, MDIELECTRIC(1.5)),
		HEND
	};
	camera cam;
	make_camera(&cam, VEC3(-2,2,1), VEC3(0,0,-1), VEC3(0,1,0), 90, (float)nx/(float)ny);
#ifdef DEBUG
	cam_print(&cam);
	wprint(world);
#endif
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col = {0, 0, 0};
			for (int s = 0; s < ns; s++) {
#ifdef DEBUG
//				printf("rfcnt=%lu riuscnt=%lu riudcnt=%lu\n", rfcnt, riuscnt, riudcnt);
#endif
				float u = ((float)i + random_f()) / (float)nx;
				float v = ((float)j + random_f()) / (float)ny;
#ifdef DEBUG
//				printf("u=%.6f v=%.6f\n", u, v);
//				printf("u=%g v=%g rfcnt=%lu riuscnt=%lu\n", u, v, rfcnt, riuscnt);
				vec3 uv = {u, v, 0};
				printf("uv=");vprint(uv);printf(" \n");
//				printf("j=%d i=%d s=%d riuscnt=%lu\n", j, i, s, riuscnt);
#endif
				ray r;
				get_ray(&cam, &r, u, v);
#ifdef DEBUG
				printf("j=%d i=%d s=%d r=", j, i, s);
				rprint(&r);
				printf(" \n");
#endif
				vec3 col0;
				color(col0, &r, world, 0);
				vadd(col, col, col0);
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
