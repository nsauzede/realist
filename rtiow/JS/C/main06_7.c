#include "conf.h"
#include <math.h>
#include <stdio.h>

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

void main06_7(conf_t c) {
  // Image
  const int image_width = IMG_WIDTH(c);
  const int image_height = IMG_HEIGHT(c);
  const scalar aspect_ratio = (scalar)image_width / image_height;
  // Camera
  const scalar viewport_height = 2.0;
  const scalar viewport_width = aspect_ratio * viewport_height;
  const scalar focal_length = 1.0;
  // printf("viewport_width=%f\n", viewport_width);
  // printf("viewport_height=%f\n", viewport_height);
  // World
  const hittable world[] = {
      HSPHERE(0, 0, -1, 0.5),
      HSPHERE(0, -100.5, -1, 100),
  };
  const int worldlen = sizeof(world) / sizeof(world[0]);

  vec3 origin = {0.0, 0.0, 0.0};
  vec3 horizontal = {viewport_width, 0.0, 0.0};
  vec3 vertical = {0.0, viewport_height, 0.0};
  vec3 lower_left_corner;
  vec3 vtmp = {0.0, 0.0, focal_length};
  vsub(lower_left_corner, origin, vtmp);
  vdiv(vtmp, horizontal, 2.0);
  vsub(lower_left_corner, lower_left_corner, vtmp);
  vdiv(vtmp, vertical, 2.0);
  vsub(lower_left_corner, lower_left_corner, vtmp);
  // Render
  uint8_t *p = PIXELS(c);
  for (int j = image_height - 1; j >= 0; --j) {
    // printf("Scanlines remaining: %d\n", j);
    for (int i = 0; i < image_width; ++i) {
      vec3 pixel_color;
      scalar u = (scalar)i / (image_width);
      scalar v = (scalar)j / (image_height);
      vec3 direction, direction0;
      vsub(direction, lower_left_corner, origin);
      vmul(direction0, u, horizontal);
      vadd(direction, direction, direction0);
      vmul(direction0, v, vertical);
      vadd(direction, direction, direction0);
      ray r;
      rmake(&r, origin, direction);
      // printf("j=%d i=%d u=%f v=%f\n", j, i, u, v);
      ray_color(pixel_color, &r, world, worldlen);
      // if (pixel_color[0] < 0.0 || pixel_color[0] >= 1.0) {
      //   printf("col0=%f\n", pixel_color[0]);
      // }
      int ir = 255.99 * pixel_color[0];
      int ig = 255.99 * pixel_color[1];
      int ib = 255.99 * pixel_color[2];
      *p++ = ir;
      *p++ = ig;
      *p++ = ib;
    }
  }
}
