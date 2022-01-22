#include "conf.h"
#include <stdio.h>

#include "vec3.h"

void main03_3(conf_t c) {
  // Image
  const int image_width = IMG_WIDTH(c);
  const int image_height = IMG_HEIGHT(c);
  // Render
  uint8_t *p = PIXELS(c);
  for (int j = image_height - 1; j >= 0; --j) {
    // printf("Scanlines remaining: %d\n", j);
    for (int i = 0; i < image_width; ++i) {
      vec3 pixel_color;
      // pixel_color[0] = (scalar)i / (image_width - 1);
      // pixel_color[1] = (scalar)j / (image_height - 1);
      pixel_color[0] = (scalar)i / (image_width);
      pixel_color[1] = (scalar)j / (image_height);
      // pixel_color[2] = 0.15;
      pixel_color[2] = 0.2;

      int ir = 255.999 * pixel_color[0];
      int ig = 255.999 * pixel_color[1];
      int ib = 255.999 * pixel_color[2];
      *p++ = ir;
      *p++ = ig;
      *p++ = ib;

      //   printf("%d %d %d\n", ir, ig, ib);
    }
  }
}
