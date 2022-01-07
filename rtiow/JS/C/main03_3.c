#include <stdio.h>

#include "vec3.h"

int main() {
    // Image
    const int image_width = 400;
    const int image_height = 225;
    // Render
    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (int j = image_height-1; j >= 0; --j) {
        fprintf(stderr, "Scanlines remaining: %d\n", j);
        for (int i = 0; i < image_width; ++i) {
            vec3 pixel_color;
            pixel_color[0] = (double)i / (image_width-1);
            pixel_color[1]  = (double)j / (image_height-1);
            pixel_color[2] = 0.25;

            int ir = 255.999 * pixel_color[0];
            int ig = 255.999 * pixel_color[1];
            int ib = 255.999 * pixel_color[2];

            printf("%d %d %d\n", ir, ig, ib);
        }
    }
}
