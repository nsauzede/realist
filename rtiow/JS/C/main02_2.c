#include <stdio.h>
int main() {
    // Image
    const int image_width = 400;
    const int image_height = 225;
    // Render
    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            double r = (double)i / (image_width-1);
            double g = (double)j / (image_height-1);
            double b = 0.25;

            int ir = 255.999 * r;
            int ig = 255.999 * g;
            int ib = 255.999 * b;

            printf("%d %d %d\n", ir, ig, ib);
        }
    }
}
