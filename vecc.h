#include <stdio.h>
#include <math.h>

typedef double v3[3];

void vcross( v3 l, const v3 r1, const v3 r2) {
	l[0] = r1[1] * r2[2] - r1[2] * r2[1];
	l[1] = r1[2] * r2[0] - r1[0] * r2[2];
	l[2] = r1[0] * r2[1] - r1[1] * r2[0];
}

void vadd( v3 l, const v3 r1, const v3 r2) {
	l[0] = r1[0] + r2[0];
	l[1] = r1[1] + r2[1];
	l[2] = r1[2] + r2[2];
}

void vsub( v3 l, const v3 r1, const v3 r2) {
	l[0] = r1[0] - r2[0];
	l[1] = r1[1] - r2[1];
	l[2] = r1[2] - r2[2];
}

void vcopy( v3 l, const double *r) {
	l[0] = r[0];
	l[1] = r[1];
	l[2] = r[2];
}

void vset( v3 l, const double r1, const double r2, const double r3) {
	l[0] = r1;
	l[1] = r2;
	l[2] = r3;
}

void vmult( v3 l, const v3 r1, const double r2) {
	l[0] = r1[0] * r2;
	l[1] = r1[1] * r2;
	l[2] = r1[2] * r2;
}

double vdot( const v3 v1, const v3 v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vnormalize( v3 l) {
	double norm;
	norm = sqrt( vdot( l, l));
	l[0] /= norm;
	l[1] /= norm;
	l[2] /= norm;
}

//inline void vprint( const v3 v) {
//	printf( "%f,%f,%f\n", v[0], v[1], v[2]);
//}

inline void vprintn( const char *t, const v3 v) {
	printf( "%s={%f,%f,%f}\n", t, v[0], v[1], v[2]);
}
