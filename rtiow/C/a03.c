#include <stdio.h>

#include "vec3.h"

int main() {
	int nx = 200;
	int ny = 100;
	printf("P3\n");
	printf("%d %d\n", nx, ny);
	printf("255\n");
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col = {(float)i / (float)nx, (float)j / (float)ny, 0.2};
			int ir = (int)(255.99*col[0]);
			int ig = (int)(255.99*col[1]);
			int ib = (int)(255.99*col[2]);
			printf("%d %d %d\n", ir, ig, ib);
		}
	}
	return 0;
}
