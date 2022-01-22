#include <stdio.h>
#include <stdlib.h>

#include "ray.h"
#include "vec3.h"

void color(vec3 col, const ray r) {
  vec3 unit_direction;
  unit_vector(unit_direction, r.direction);
  //   printf("unit_direction: %f,%f,%f\n", unit_direction[0],
  //   unit_direction[1],
  //          unit_direction[2]);
  float t = 0.5 * (unit_direction[1] + 1.0);
  vec3 col0 = {1.0, 1.0, 1.0};
  vec3 col1 = {0.5, 0.7, 1.0};
  vmul(col0, 1.0 - t, col0);
  vmul(col1, t, col1);
  vadd(col, col0, col1);
}

int main() {
  int nx = 200;
  int ny = 100;
  printf("P3\n");
  printf("%d %d\n", nx, ny);
  printf("255\n");
  vec3 lower_left_corner = {-2.0, -1.0, -1.0};
  //	vprint(lower_left_corner);
  vec3 horizontal = {4.0, 0.0, 0.0};
  vec3 vertical = {0.0, 2.0, 0.0};
  vec3 origin = {0.0, 0.0, 0.0};
  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      float u = (float)i / (float)nx;
      float v = (float)j / (float)ny;
      //   printf("j=%d i=%d u=%f v=%f\n", j, i, u, v);
      ray r;
      vec3 direction, direction0, direction1;
      vmul(direction0, v, vertical);
      vmul(direction1, u, horizontal);
      vadd(direction, direction0, direction1);
      vadd(direction, direction, lower_left_corner);
      rmake(&r, origin, direction);
      //			rprint(&r);exit(0);
      vec3 col;
      color(col, r);
      //   printf("%f %f %f\n", col[0], col[1], col[2]);
      int ir = (int)(255.99f * col[0]);
      int ig = (int)(255.99f * col[1]);
      int ib = (int)(255.99f * col[2]);
      printf("%d %d %d\n", ir, ig, ib);
    }
  }
  return 0;
}
