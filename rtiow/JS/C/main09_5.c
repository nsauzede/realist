#include "conf.h"
#include <math.h>
#include <stdio.h>

#define RANDOM_IMPL
#include "random.h"
#include "ray.h"
#include "vec3.h"

typedef struct {
  vec3 albedo;
} mlambertian_t;

typedef struct {
  vec3 albedo;
} mmetal_t;

typedef struct {
  enum { Lambertian, Metal } type;
  union {
    mlambertian_t lambertian;
    mmetal_t metal;
  };
} material_t;

typedef struct {
  vec3 center;
  scalar radius;
  const material_t *mat;
} sphere_t;

typedef struct {
  enum { Sphere } type;
  union {
    sphere_t sphere;
  };
} hittable;

#define MLAMBERTIAN(r, g, b)                                                   \
  ((material_t){Lambertian, .lambertian = (mlambertian_t){r, g, b}})

#define MMETAL(r, g, b) ((material_t){Metal, .metal = (mmetal_t){r, g, b}})

#define HSPHERE(cx, cy, cz, r, m)                                              \
  ((hittable){Sphere, .sphere = (sphere_t){{cx, cy, cz}, r, m}})

typedef struct {
  vec3 p;
  vec3 normal;
  const material_t *mat;
  scalar t;
  int front_face;
} hit_record;

typedef struct {
  vec3 origin;
  vec3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
} camera;

static inline void set_face_normal(hit_record *hr, const ray *r,
                                   const vec3 outward_normal) {
  if (vdot(r->direction, outward_normal) < 0) {
    hr->front_face = 1;
    vcopy(hr->normal, outward_normal);
  } else {
    hr->front_face = 0;
    vmul(hr->normal, -1, outward_normal);
  }
}

static int hit_sphere(const sphere_t s, const ray *r, scalar min, scalar max,
                      hit_record *rec) {
  vec3 oc;
  vsub(oc, r->origin, s.center);
  scalar a = vsqlen(r->direction);
  scalar hb = vdot(oc, r->direction);
  scalar c = vsqlen(oc) - s.radius * s.radius;
  scalar discriminant = hb * hb - a * c;
  if (discriminant < 0.0) {
    return 0;
  }
  scalar sqrtd = ssqrt(discriminant);
  scalar root = (-hb - sqrtd) / a;
  if (root < min || root > max) {
    root = (-hb + sqrtd) / a;
    if (root < min || root > max) {
      return 0;
    }
  }
  rec->t = root;
  point_at_parameter(rec->p, r, root);
  vsub(rec->normal, rec->p, s.center);
  vdiv(rec->normal, rec->normal, s.radius);
  rec->mat = s.mat;
  return 1;
}

static int hit(const hittable *world, int worldlen, const ray *r, scalar min,
               scalar max, hit_record *rec) {
  hit_record temp_rec;
  int hit_anything = 0;
  scalar closest_so_far = max;
  for (int i = 0; i < worldlen; i++) {
    if (hit_sphere(world[i].sphere, r, min, closest_so_far, &temp_rec)) {
      hit_anything = 1;
      closest_so_far = temp_rec.t;
      *rec = temp_rec;
    }
  }
  return hit_anything;
}

static int lambertian_scatter(vec3 attenuation, ray *scattered, const ray *r,
                              const hit_record *rec) {
  vec3 scatter_direction;
  random_in_unit_sphere(scatter_direction);
  vadd(scatter_direction, rec->normal, scatter_direction);
  if (near_zero(scatter_direction)) {
    vcopy(scatter_direction, rec->normal);
  }
  rmake(scattered, rec->p, scatter_direction);
  vcopy(attenuation, rec->mat->lambertian.albedo);
  return 1;
}

static void reflect(vec3 l, const vec3 v, const vec3 n) {
  vmul(l, 2.0 * vdot(v, n), n);
  vsub(l, v, l);
}

static int metal_scatter(vec3 attenuation, ray *scattered, const ray *r_in,
                         const hit_record *rec) {
  vec3 uvector, reflected;
  unit_vector(uvector, r_in->direction);
  reflect(reflected, uvector, rec->normal);
  rmake(scattered, rec->p, reflected);
  vcopy(attenuation, rec->mat->metal.albedo);
  return vdot(scattered->direction, rec->normal) > 0;
}

static int scatter(vec3 attenuation, ray *scattered, const ray *r,
                   const hit_record *rec) {
  if (rec->mat->type == Lambertian)
    return lambertian_scatter(attenuation, scattered, r, rec);
  else if (rec->mat->type == Metal)
    return metal_scatter(attenuation, scattered, r, rec);
  return 0;
}

static void ray_color(vec3 col, const ray *r, const hittable *world,
                      int worldlen, int depth) {
  if (depth <= 0) {
    vcopy(col, (vec3){0.0, 0.0, 0.0});
    return;
  }
  hit_record rec;
  // if (hit(world, worldlen, r, 0.001, INFINITY, &rec)) {
  if (hit(world, worldlen, r, 0.001, FLT_MAX, &rec)) {
    ray scattered;
    vec3 attenuation;
    if (scatter(attenuation, &scattered, r, &rec)) {
      ray_color(col, &scattered, world, worldlen, depth - 1);
      vmulv(col, attenuation, col);
      return;
    }
    vcopy(col, (vec3){0.0, 0.0, 0.0});
    return;
  }
  vec3 unit_direction;
  unit_vector(unit_direction, r->direction);
  scalar t = 0.5 * (unit_direction[1] + 1.0);
  vec3 col0 = {1.0, 1.0, 1.0};
  vec3 col1 = {0.5, 0.7, 1.0};
  vmul(col0, 1.0 - t, col0);
  vmul(col1, t, col1);
  vadd(col, col0, col1);
}

static inline scalar clamp(scalar x, scalar min, scalar max) {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}

static inline void make_camera(camera *cam) {
  // const scalar aspect_ratio = 16.0 / 9.0;
  const scalar aspect_ratio = 2.0;
  const scalar viewport_height = 2.0;
  const scalar viewport_width = aspect_ratio * viewport_height;
  const scalar focal_length = 1.0;
  // printf("viewport_width=%f\n", viewport_width);
  // printf("viewport_height=%f\n", viewport_height);
  vcopy(cam->origin, (vec3){0.0, 0.0, 0.0});
  vcopy(cam->horizontal, (vec3){viewport_width, 0.0, 0.0});
  vcopy(cam->vertical, (vec3){0.0, viewport_height, 0.0});
  vec3 vtmp = {0.0, 0.0, focal_length};
  vsub(cam->lower_left_corner, cam->origin, vtmp);
  vdiv(vtmp, cam->horizontal, 2.0);
  vsub(cam->lower_left_corner, cam->lower_left_corner, vtmp);
  vdiv(vtmp, cam->vertical, 2.0);
  vsub(cam->lower_left_corner, cam->lower_left_corner, vtmp);
}

static inline void get_ray(ray *r, const camera *cam, scalar u, scalar v) {
  vec3 direction, direction0;
  vsub(direction, cam->lower_left_corner, cam->origin);
  vmul(direction0, u, cam->horizontal);
  vadd(direction, direction, direction0);
  vmul(direction0, v, cam->vertical);
  vadd(direction, direction, direction0);
  rmake(r, cam->origin, direction);
}

void main09_5(conf_t c) {
  pcg_srand(0);
  // Image
  const int image_width = IMG_WIDTH(c);
  const int image_height = IMG_HEIGHT(c);
  const int samples_per_pixel = 100;
  const int max_depth = 50;
  const scalar scale = 1.0 / samples_per_pixel;
  // Camera
  camera cam;
  make_camera(&cam);
  // World
  const material_t mat_ground = MLAMBERTIAN(0.8, 0.8, 0.0);
  const material_t mat_center = MLAMBERTIAN(0.7, 0.3, 0.3);
  const material_t mat_left = MMETAL(0.8, 0.8, 0.8);
  const material_t mat_right = MMETAL(0.8, 0.6, 0.2);
  const hittable world[] = {
      HSPHERE(0.0, -100.5, -1.0, 100.0, &mat_ground),
      HSPHERE(0.0, 0.0, -1.0, 0.5, &mat_center),
      HSPHERE(-1.0, 0.0, -1.0, 0.5, &mat_left),
      HSPHERE(1.0, 0.0, -1.0, 0.5, &mat_right),
  };
  const int worldlen = sizeof(world) / sizeof(world[0]);
  // Render
  uint8_t *p = PIXELS(c);
  for (int j = image_height - 1; j >= 0; --j) {
    // printf("Scanlines remaining: %d\n", j);
    for (int i = 0; i < image_width; ++i) {
      vec3 pixel_color = {0.0, 0.0, 0.0};
      for (int s = 0; s < samples_per_pixel; s++) {
        scalar u = (i + random_s()) / (image_width);
        scalar v = (j + random_s()) / (image_height);
        // printf("j=%d i=%d u=%f v=%f\n", j, i, u, v);
        ray r;
        get_ray(&r, &cam, u, v);
        // rprint(&r);
        // printf("\n");
        vec3 rcolor;
        ray_color(rcolor, &r, world, worldlen, max_depth);
        vadd(pixel_color, pixel_color, rcolor);
      }
      vmul(pixel_color, scale, pixel_color);
      int ir = 255.99 * clamp(ssqrt(pixel_color[0]), 0.0, 0.999);
      int ig = 255.99 * clamp(ssqrt(pixel_color[1]), 0.0, 0.999);
      int ib = 255.99 * clamp(ssqrt(pixel_color[2]), 0.0, 0.999);
      // printf("%d %d %d\n", ir, ig, ib);
      *p++ = ir;
      *p++ = ig;
      *p++ = ib;
    }
  }
}
