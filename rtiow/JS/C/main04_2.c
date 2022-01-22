#include "conf.h"
#include <stdio.h>

#include "ray.h"
#include "vec3.h"

static void ray_color(vec3 col, const ray r) {
  vec3 unit_direction;
  unit_vector(unit_direction, r.direction);
  // printf("unit_direction: %f,%f,%f\n", unit_direction[0], unit_direction[1],
  //        unit_direction[2]);
  scalar t = 0.5 * (unit_direction[1] + 1.0);
  // printf("t=%f\n", t);
  vec3 col0 = {1.0, 1.0, 1.0};
  vec3 col1 = {0.5, 0.7, 1.0};
  vmul(col0, 1.0 - t, col0);
  vmul(col1, t, col1);
  vadd(col, col0, col1);
}

void main04_2(conf_t c) {
  // Image
  const int image_width = IMG_WIDTH(c);
  const int image_height = IMG_HEIGHT(c);
  const scalar aspect_ratio = (scalar)image_width / image_height;
  // Camera
  const scalar viewport_height = 2.0;
  const scalar viewport_width = aspect_ratio * viewport_height;
  const scalar focal_length = 1.0;
  // printf("aspect_ratio=%f\n", aspect_ratio);
  // printf("viewport_width=%f\n", viewport_width);
  // printf("viewport_height=%f\n", viewport_height);

#if 0
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
#else
  vec3 lower_left_corner = {-2.0, -1.0, -1.0};
  //	vprint(lower_left_corner);
  vec3 horizontal = {4.0, 0.0, 0.0};
  vec3 vertical = {0.0, 2.0, 0.0};
  vec3 origin = {0.0, 0.0, 0.0};
#endif
  // printf("origin: %f,%f,%f\n", origin[0], origin[1], origin[2]);
  // printf("horizontal: %f,%f,%f\n", horizontal[0], horizontal[1],
  // horizontal[2]); printf("vertical: %f,%f,%f\n", vertical[0], vertical[1],
  // vertical[2]); printf("lower_left_corner: %f,%f,%f\n", lower_left_corner[0],
  //        lower_left_corner[1], lower_left_corner[2]);
  // Render
  uint8_t *p = PIXELS(c);
  for (int j = image_height - 1; j >= 0; --j) {
    // printf("Scanlines remaining: %d\n", j);
    for (int i = 0; i < image_width; ++i) {
      vec3 pixel_color;
      // scalar u = (scalar)i / (image_width - 1);
      // scalar v = (scalar)j / (image_height - 1);
      scalar u = (scalar)i / (image_width);
      scalar v = (scalar)j / (image_height);
      // printf("j=%d i=%d u=%f v=%f\n", j, i, u, v);
      vec3 direction, direction0;
      vsub(direction, lower_left_corner, origin);
      vmul(direction0, v, vertical);
      vadd(direction, direction, direction0);
      vmul(direction0, u, horizontal);
      vadd(direction, direction, direction0);
      ray r;
      rmake(&r, origin, direction);
      // printf("direction: %f,%f,%f\n", direction[0], direction[1],
      // direction[2]);
      ray_color(pixel_color, r);
      // printf("%f %f %f\n", pixel_color[0], pixel_color[1], pixel_color[2]);
      int ir = 255.99 * pixel_color[0];
      int ig = 255.99 * pixel_color[1];
      int ib = 255.99 * pixel_color[2];
      // printf("%d %d %d\n", ir, ig, ib);
      *p++ = ir;
      *p++ = ig;
      *p++ = ib;
    }
  }
}
