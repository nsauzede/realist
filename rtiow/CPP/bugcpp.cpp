#include <stdio.h>
int main() {
	float v1[3] = {9.000000,1002.000000,2.600000};
	float v2[3] = {-6.562669,-0.491479,-0.021382};
	printf("DOT %f,%f,%f;%f,%f,%f",
		v1[0], v1[1], v1[2],
		v2[0], v2[1], v2[2]
	);
	float ret = v1[0] *v2[0] + v1[1] *v2[1]  + v1[2] *v2[2];
	printf(" => %f\n", ret);
	return 0;
}
