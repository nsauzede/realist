#include "conf.h"
#include <math.h>
#include <stdio.h>

#include "ray.h"
#include "vec3.h"

static double hit_sphere(const vec3 center, double radius, const ray *r) {
  vec3 oc;
  vsub(oc, r->origin, center);
  double a = vdot(r->direction, r->direction);
  double b = 2.0 * vdot(oc, r->direction);
  double c = vdot(oc, oc) - radius * radius;
  double discriminant = b * b - 4.0 * a * c;
  if (discriminant < 0.0) {
    return -1.0;
  } else {
    // printf("a=%f b=%f c=%f d=%f\n", a, b, c, discriminant);
    return (-b - ssqrt(discriminant)) / (2.0 * a);
  }
}

static void ray_color(vec3 col, const ray *r) {
  double t = hit_sphere((vec3){0.0, 0.0, -1.0}, 0.5, r);
  if (t > 0.0) {
    // printf("t=%f\n", t);
    vec3 N;
    point_at_parameter(N, r, t);
    vsub(N, N, (vec3){0.0, 0.0, -1.0});
    unit_vector(N, N);
    vadd(N, N, (vec3){1.0, 1.0, 1.0});
    vmul(col, 0.5, N);
    return;
  }
  vec3 unit_direction;
  unit_vector(unit_direction, r->direction);
  t = 0.5 * (unit_direction[1] + 1.0);
  vec3 col0 = {1.0, 1.0, 1.0};
  vec3 col1 = {0.5, 0.7, 1.0};
  vmul(col0, 1.0 - t, col0);
  vmul(col1, t, col1);
  vadd(col, col0, col1);
}

void main06_1(conf_t c) {
  // Image
  const int image_width = IMG_WIDTH(c);
  const int image_height = IMG_HEIGHT(c);
  const double aspect_ratio = (double)image_width / image_height;
  // Camera
  const double viewport_height = 2.0;
  const double viewport_width = aspect_ratio * viewport_height;
  const double focal_length = 1.0;
  // printf("viewport_width=%f\n", viewport_width);
  // printf("viewport_height=%f\n", viewport_height);
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
      double u = (double)i / (image_width);
      double v = (double)j / (image_height);
      vec3 direction, direction0;
      vsub(direction, lower_left_corner, origin);
      vmul(direction0, u, horizontal);
      vadd(direction, direction, direction0);
      vmul(direction0, v, vertical);
      vadd(direction, direction, direction0);
      ray r;
      rmake(&r, origin, direction);
      // printf("j=%d i=%d u=%f v=%f\n", j, i, u, v);
      ray_color(pixel_color, &r);
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
