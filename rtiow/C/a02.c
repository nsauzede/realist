#include <stdio.h>

int main() {
	int nx = 200;
	int ny = 100;
	printf("P3\n");
	printf("%d %d\n", nx, ny);
	printf("255\n");
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			float r = (float)i / (float)nx;
			float g = (float)j / (float)ny;
			float b = 0.2;
			int ir = (int)(255.99f*r);
			int ig = (int)(255.99f*g);
			int ib = (int)(255.99f*b);
			printf("%d %d %d\n", ir, ig, ib);
		}
	}
	return 0;
}
