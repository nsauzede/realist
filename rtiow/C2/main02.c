#include <stdio.h>

int main() {
    int nx = 200, ny = 100;
    printf("P3\n%d %d\n255\n", nx, ny);
    for (int j = ny -1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            float r = (float)i / nx;
            float g = (float)j / ny;
            float b = 0.2;
            int ir = 255.99 * r;
            int ig = 255.99 * g;
            int ib = 255.99 * b;
            printf("%d %d %d\n", ir, ig, ib);
        }
    }
    return 0;
}
