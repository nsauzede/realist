#include <stdio.h>

int main() {
    int nx = 200, ny = 100;
    printf("P3\n%d %d\n255\n", nx, ny);
    for (int j = ny -1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            float col[3] = {(float)i / nx, (float)j / ny, 0.2};
            int ir = 255.99 * col[0];
            int ig = 255.99 * col[1];
            int ib = 255.99 * col[2];
            printf("%d %d %d\n", ir, ig, ib);
        }
    }
    return 0;
}
