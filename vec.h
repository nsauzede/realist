#include <math.h>

int solvetri( const double a, const double b, const double c, double *t1, double *t2) {
	int result;
	double d = b * b - 4 * a * c;
	if (d > 0) {
		double sd = sqrt( d);
		*t1 = (-b - sd) / 2 / a;
		*t2 = (-b + sd) / 2 / a;
		result = 2;
	}
	else if (d == 0) {
		*t1 = -b / 2 / a;
		result = 1;
	}
	else {
		result = 0;
	}
	return result;
}
