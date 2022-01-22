#define CONF_IMPL
#include "conf.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]) {
  int n = 0;
  // Image
  const int image_width = 200;
  const int image_height = 100;
  int arg = 1;
  if (arg < argc) {
    int nn = -1;
    for (int i = 0; i < napis; i++) {
      if (!strcmp(argv[arg], apis[i].name)) {
        nn = i;
        break;
      }
    }
    if (nn >= 0) {
      n = nn;
    } else {
      sscanf(argv[arg++], "%d", &n);
    }
  }
  if (n < 0 || n >= napis) {
    printf("Usage: %s [n] (max n=%d)\n", argv[0], napis - 1);
    exit(1);
  }
  // Render
  printf("P3\n%d %d\n255\n", image_width, image_height);
  conf_t c = {
      image_width,
      image_height,
  };
  c.pixels = calloc(image_width * image_height, 3);
  apis[n].cb(c);
  uint8_t *p = c.pixels;
  for (int j = image_height - 1; j >= 0; --j) {
    for (int i = 0; i < image_width; ++i) {
      int ir = *p++;
      int ig = *p++;
      int ib = *p++;
      printf("%d %d %d\n", ir, ig, ib);
    }
  }
}
