#include "conf.h"
#include <math.h>
#include <stdio.h>

#define RANDOM_IMPL
#include "random.h"
#include "ray.h"
#include "vec3.h"

typedef struct {
  vec3 center;
  scalar radius;
} sphere_t;

typedef struct {
  enum { Sphere } type;
  union {
    sphere_t sphere;
  };
} hittable;
#define HSPHERE(cx, cy, cz, r)                                                 \
  ((hittable){Sphere, .sphere = (sphere_t){{cx, cy, cz}, r}})

typedef struct {
  vec3 p;
  vec3 normal;
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

static void ray_color(vec3 col, const ray *r, const hittable *world,
                      int worldlen) {
  hit_record rec;
  if (hit(world, worldlen, r, 0, INFINITY, &rec)) {
    vadd(col, rec.normal, (vec3){1.0, 1.0, 1.0});
    vmul(col, 0.5, col);
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
  const scalar aspect_ratio = 16.0 / 9.0;
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

void main07_2(conf_t c) {
  pcg_srand(0);
  // Image
  const int image_width = IMG_WIDTH(c);
  const int image_height = IMG_HEIGHT(c);
  const int samples_per_pixel = 100;
  const scalar scale = 1.0 / samples_per_pixel;
  // Camera
  camera cam;
  make_camera(&cam);
  // World
  const hittable world[] = {
      HSPHERE(0, 0, -1, 0.5),
      HSPHERE(0, -100.5, -1, 100),
  };
  const int worldlen = sizeof(world) / sizeof(world[0]);
  // Render
  uint8_t *p = PIXELS(c);
  for (int j = image_height - 1; j >= 0; --j) {
    // printf("Scanlines remaining: %d\n", j);
    for (int i = 0; i < image_width; ++i) {
      vec3 pixel_color = {0.0, 0.0, 0.0};
      for (int s = 0; s < samples_per_pixel; s++) {
        scalar u = (i + random_s()) / (image_width - 1);
        scalar v = (j + random_s()) / (image_height - 1);
        ray r;
        get_ray(&r, &cam, u, v);
        // printf("j=%d i=%d u=%f v=%f\n", j, i, u, v);
        vec3 rcolor;
        ray_color(rcolor, &r, world, worldlen);
        vadd(pixel_color, pixel_color, rcolor);
      }
      vmul(pixel_color, scale, pixel_color);
      int ir = 255.999 * clamp(pixel_color[0], 0.0, 0.999);
      int ig = 255.999 * clamp(pixel_color[1], 0.0, 0.999);
      int ib = 255.999 * clamp(pixel_color[2], 0.0, 0.999);
      *p++ = ir;
      *p++ = ig;
      *p++ = ib;
    }
  }
}
