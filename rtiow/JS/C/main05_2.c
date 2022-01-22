#include "conf.h"
#include <stdio.h>

#include "ray.h"
#include "vec3.h"

static int hit_sphere(const vec3 center, scalar radius, const ray r) {
  vec3 oc;
  vsub(oc, r.origin, center);
  scalar a = vdot(r.direction, r.direction);
  scalar b = 2.0 * vdot(oc, r.direction);
  scalar c = vdot(oc, oc) - radius * radius;
  scalar discriminant = b * b - 4.0 * a * c;
  return discriminant > 0.0;
}

static void ray_color(vec3 col, const ray r) {
  if (hit_sphere((vec3){0.0, 0.0, -1.0}, 0.5, r)) {
    col[0] = 1.0;
    col[1] = 0.0;
    col[2] = 0.0;
    return;
  }
  vec3 unit_direction;
  unit_vector(unit_direction, r.direction);
  scalar t = 0.5 * (unit_direction[1] + 1.0);
  vec3 col0 = {1.0, 1.0, 1.0};
  vec3 col1 = {0.5, 0.7, 1.0};
  vmul(col0, 1.0 - t, col0);
  vmul(col1, t, col1);
  vadd(col, col0, col1);
}

void main05_2(conf_t c) {
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
      scalar u = (scalar)i / (image_width - 1);
      scalar v = (scalar)j / (image_height - 1);
      vec3 direction, direction0;
      vsub(direction, lower_left_corner, origin);
      vmul(direction0, u, horizontal);
      vadd(direction, direction, direction0);
      vmul(direction0, v, vertical);
      vadd(direction, direction, direction0);
      ray r;
      rmake(&r, origin, direction);
      ray_color(pixel_color, r);
      int ir = 255.999 * pixel_color[0];
      int ig = 255.999 * pixel_color[1];
      int ib = 255.999 * pixel_color[2];
      *p++ = ir;
      *p++ = ig;
      *p++ = ib;
    }
  }
}
