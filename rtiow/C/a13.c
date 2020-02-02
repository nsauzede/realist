#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <float.h>

#include "vec3.h"
#include "ray.h"

#ifdef DEBUG
unsigned long rfcnt;
unsigned long riuscnt;
unsigned long riudcnt;
#define INLINE
#else
#define INLINE static inline
#endif

INLINE float random_f() {
#ifdef DEBUG
	rfcnt++;
#endif
	return (float)rand() / ((float)RAND_MAX + (float)1.0);
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
	} while (vsqlen(p) >= 1.0);
}

void random_in_unit_disk(vec3 p) {
#ifdef DEBUG
	riudcnt++;
#endif
	do {
		float r1 = random_f();
		float r2 = random_f();
		vmul(p, 2.0, VEC3(r1,r2,0));
		vsub(p, p, VEC3(1,1,0));
	} while (vsqlen(p) >= 1.0);
}

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

#define HSPHERE(cx, cy, cz, r, m) ((hittable_t){sphere_hit, .u.sphere=(sphere_t){{cx, cy, cz}, r, m}})
#define HSPHEREV(c, r, m) ((hittable_t){sphere_hit, .u.sphere=(sphere_t){c, r, m}})
#define HSTART ((hittable_t){list_hit, 0})
#define HEND ((hittable_t){0, 0})

struct hittable_s;
typedef bool (*hit_t)(struct hittable_s *p, const ray *r, float t_min, float t_max, hit_record *rec);

typedef struct hittable_s {
	hit_t hit;
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
	vmul(l, (float)2. * vdot(v, n), n);
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
	float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
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
		ni_over_nt = 1.0 / d->ref_idx;
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
	if (!world->hit) return;
	hit_record rec;
	// remove acne by starting at 0.001
#ifdef DEBUG
	rprint(r);
#endif
	if (world->hit(world, r, 0.001, FLT_MAX, &rec)) {
#ifdef DEBUG
		printf("HIT\n");
#endif
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
	vec3 u, v, w;
	float lens_radius;
} camera;

void cam_print(const camera *cam) {
	printf("Origin: ");vprint(cam->origin);
	printf("\nLower_left: ");vprint(cam->lower_left_corner);
	printf("\nhorizontal: ");vprint(cam->horizontal);
	printf("\nvertical: ");vprint(cam->vertical);
	printf("\nu: ");vprint(cam->u);
	printf("\nv: ");vprint(cam->v);
	printf("\nw: ");vprint(cam->w);
	printf("\nlens_radius=%f\n", cam->lens_radius);
}

// vfov is top to bottom in degrees
void make_camera(camera *cam, const vec3 lookfrom, const vec3 lookat,
		const vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
	cam->lens_radius = aperture / 2;
	float theta = vfov*M_PI/180;
	float half_height = tanf(theta/2);
	float half_width = aspect * half_height;

	vcopy(cam->origin, lookfrom);
	vsub(cam->w, lookfrom, lookat);
	unit_vector(cam->w, cam->w);
	vcross(cam->u, vup, cam->w);
	unit_vector(cam->u, cam->u);
	vcross(cam->v, cam->w, cam->u);

	vec3 tmp;
	vmul(tmp, half_width * focus_dist, cam->u);
	vsub(cam->lower_left_corner, cam->origin, tmp);
	vmul(tmp, half_height * focus_dist, cam->v);
	vsub(cam->lower_left_corner, cam->lower_left_corner, tmp);
	vmul(tmp, focus_dist, cam->w);
	vsub(cam->lower_left_corner, cam->lower_left_corner, tmp);
	vmul(cam->horizontal, 2 * half_width * focus_dist, cam->u);
	vmul(cam->vertical, 2 * half_height * focus_dist, cam->v);
}

void get_ray(camera *cam, ray *r, float s, float t) {
#ifdef DEBUG
	printf("s=%g t=%g\n", s, t);
#endif
	vec3 orig, direction, tmp0, tmp1;
	vec3 rd, offset;
	random_in_unit_disk(rd);
	vmul(rd, cam->lens_radius, rd);
	vmul(tmp0, rd[0], cam->u);
	vmul(tmp1, rd[1], cam->v);
	vadd(offset, tmp0, tmp1);
	vmul(tmp0, s, cam->horizontal);
	vmul(tmp1, t, cam->vertical);
	vadd(direction, tmp0, tmp1);
	vadd(direction, cam->lower_left_corner, direction);
	vadd(orig, cam->origin, offset);
	vsub(direction, direction, orig);
	rmake(r, orig, direction);
}

hittable_t *random_scene() {
	int n = 500;
	hittable_t *list = malloc((n + 3)* sizeof(hittable_t));
	int i = 0;
	list[i++] = HSTART;
	list[i++] = HSPHERE(0,-1000,0, 1000, MLAMBERTIAN(0.5, 0.5, 0.5));
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = random_f();
			float r1 = random_f();
			float r2 = random_f();
			vec3 center = {a + 0.9 * r1, 0.2, b + 0.9 * r2};
			vec3 tmp;
			vsub(tmp, center, VEC3(4, 0.2, 0));
			if (vlen(tmp) > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					float r1, r2, r3, r4, r5, r6;
					r1 = random_f();
					r2 = random_f();
					r3 = random_f();
					r4 = random_f();
					r5 = random_f();
					r6 = random_f();
list[i++] = HSPHERE(center[0], center[1], center[2], 0.2,
MLAMBERTIAN(r1 * r2, r3 * r4, r5 * r6));
				} else if (choose_mat < 0.95) { // metal
					float r1, r2, r3, r4;
					r1 = .5 * (1 + random_f());
					r2 = .5 * (1 + random_f());
					r3 = .5 * (1 + random_f());
					r4 = .5 * random_f();
list[i++] = HSPHERE(center[0], center[1], center[2], 0.2,
MMETAL(r1, r2, r3, r4));
				} else {  // glass
list[i++] = HSPHERE(center[0], center[1], center[2], 0.2,
MDIELECTRIC(1.5));
				}
			}
		}
	}
	list[i++] = HSPHERE(0, 1, 0, 1.0, MDIELECTRIC(1.5));
	list[i++] = HSPHERE(-4, 1, 0, 1.0, MLAMBERTIAN(0.4, 0.2, 0.1));
	list[i++] = HSPHERE(4, 1, 0, 1.0, MMETAL(0.7, 0.6, 0.5, 0.0));
	list[i++] = HEND;
	return list;
}

int main() {
	srand(0);
	int nx = 200;
	int ny = 100;
	int ns = 4;
	printf("P3\n"); printf("%d %d\n", nx, ny); printf("255\n");
	float R = cos(M_PI/4);
	hittable_t *world = random_scene();
	vec3 lookfrom = {9,2,2.6};
	vec3 lookat = {3,.8,1};
	float dist_to_focus;
	float aperture = 0.0;
	camera cam;
	vec3 vtmp;
	vsub(vtmp, lookfrom, lookat);
	dist_to_focus = vlen(vtmp);
	make_camera(&cam, lookfrom, lookat, VEC3(0,1,0), 30,
		(float)nx/(float)ny, aperture, dist_to_focus);
#ifdef DEBUG
	cam_print(&cam);
#endif
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col = {0, 0, 0};
			for (int s = 0; s < ns; s++) {
#ifdef DEBUG
printf("rfcnt=%lu riuscnt=%lu riudcnt=%lu\n", rfcnt, riuscnt, riudcnt);
#endif
				float u = ((float)i + random_f()) / (float)nx;
				float v = ((float)j + random_f()) / (float)ny;
#ifdef DEBUG
				printf("u=%g v=%g\n", u, v);
#endif
				ray r;
				get_ray(&cam, &r, u, v);
				vec3 col0;
				color(col0, &r, world, 0);
				vadd(col, col, col0);
			}
			vdiv(col, col, (float)ns);
			// Gamma 2 correction (square root)
			col[0] = sqrtf(col[0]);
			col[1] = sqrtf(col[1]);
			col[2] = sqrtf(col[2]);

			int ir = (int)(255.99*col[0]);
			int ig = (int)(255.99*col[1]);
			int ib = (int)(255.99*col[2]);
			printf("%d %d %d\n", ir, ig, ib);
		}
	}
}
