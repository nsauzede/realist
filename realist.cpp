#include <stdio.h>
#include <math.h>
#include <vector>

#include "CSDL.h"

typedef double v3[3];

void vcross( v3 &l, const v3 &r1, const v3 &r2) {
	l[0] = r1[1] * r2[2] - r1[2] * r2[1];
	l[1] = r1[2] * r2[0] - r1[0] * r2[2];
	l[2] = r1[0] * r2[1] - r1[1] * r2[0];
}

void vadd( v3 &l, const v3 &r1, const v3 &r2) {
	l[0] = r1[0] + r2[0];
	l[1] = r1[1] + r2[1];
	l[2] = r1[2] + r2[2];
}

void vsub( v3 &l, const v3 &r1, const v3 &r2) {
	l[0] = r1[0] - r2[0];
	l[1] = r1[1] - r2[1];
	l[2] = r1[2] - r2[2];
}

void vcopy( v3 &l, const double *r) {
	l[0] = r[0];
	l[1] = r[1];
	l[2] = r[2];
}

void vset( v3 &l, const double &r1, const double &r2, const double &r3) {
	l[0] = r1;
	l[1] = r2;
	l[2] = r3;
}

void vmult( v3 &l, const v3 &r1, const double &r2) {
	l[0] = r1[0] * r2;
	l[1] = r1[1] * r2;
	l[2] = r1[2] * r2;
}

void vnorm( v3 &v) {
	double norm;
	norm = sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= norm;
	v[1] /= norm;
	v[2] /= norm;
}

double vdot( const v3 &v1, const v3 &v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vprint( const char *t, const v3 &v) {
	printf( "%s={%f,%f,%f}\n", t, v[0], v[1], v[2]);
}

void vprint( const v3 &v) {
	printf( "%f,%f,%f\n", v[0], v[1], v[2]);
}

int solvetri( const double &a, const double &b, const double &c, double &t1, double &t2) {
	int result;
	double d = b * b - 4 * a * c;
	if (d > 0) {
		double sd = sqrt( d);
		t1 = (-b - sd) / 2 / a;
		t2 = (-b + sd) / 2 / a;
		result = 2;
	}
	else if (d == 0) {
		t1 = -b / 2 / a;
		result = 1;
	}
	else {
		result = 0;
	}
	return result;
}

class CObject {
public:
	virtual ~CObject() {
	}
	// returns intersection distance (HUGE_VAL => no intersection)
	virtual double Intersec( const v3 &e, const v3 &v) const = 0;
	virtual void SetColor( const double *color) {
		vcopy( m_color, color);
	}
	virtual const v3 &Color() const {
		return m_color;
	}
private:
	v3 m_color;
};

class CSphere : public CObject {
public:
	// { center_x, center_y, center_z, radius, color_r, color_g, color_b }
	enum {
		CENTER_X, CENTER_Y, CENTER_Z, RADIUS, COLOR, COLOR_RED = COLOR, COLOR_GREEN, COLOR_BLUE,
		A0, B0, C0, D0, E0, F0,
		MAX
	};
	CSphere( const double *params) :
		m_r( params[RADIUS]) {
		vcopy( m_c, params);
		SetColor( &params[COLOR]);
	}
	double Intersec( const v3 &e, const v3 &v) const {
		double result = HUGE_VAL;
		double sr2 = m_r * m_r;
		v3 t;
		vsub( t, e, m_c);
		double a, b, c;
		a = vdot( v, v);
		b = 2 * vdot( v, t);
		c = vdot( t, t) - sr2;
		double t1, t2;
		int sol = solvetri( a, b, c, t1, t2);
		if (sol >= 1) {
			result = t1;
		}
		return result;
	}
private:
	v3 m_c;
	double m_r;
};

class CRealist {
public:
	void Trace( const v3 &v, v3 &color) const {
		v3 va;
		vadd( va, m_e, v);
#define TMAX 1E10
		double tmin = HUGE_VAL;
		CObject *omin = 0;
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			double tres = m_objs.at( ii)->Intersec( m_e, v);
			if (tres < tmin) {
				tmin = tres;
				omin = m_objs.at( ii);
			}
		}
		vmult( color, color, 0.2);
		if (tmin < TMAX) {
			vcopy( color, omin->Color());
		}
	}
	void Render( const double &tr = 0) const {
		// ray
		v3 v;
		printf( "tr=%f\n", tr);
		for (unsigned jj = 0; jj < m_h; jj++) {
			v3 vu;
			vmult( vu, m_u, ((double)jj - m_h / 2) / m_h * m_hh);
			for (unsigned ii = 0; ii < m_w; ii++) {
				v3 vr;
				vmult( vr, m_r, ((double)ii - m_w / 2) / m_w * m_ww);
				vcopy( v, m_f);
				vnorm( v);
				vadd( v, v, vu);
				vadd( v, v, vr);
				v3 color = { 1, 1, 1};
				Trace( v, color);
				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 0] = color[0];
				m_arr[(((m_h - jj - 1)  * m_w + ii) * 3) + 1] = color[1];
				m_arr[(((m_h - jj - 1)  * m_w + ii) * 3) + 2] = color[2];
			}
		}
	}
#define W 640
#define H 480
	void Run( unsigned w = W, unsigned h = H) {
		CSDL sdl;
		m_w = w;
		m_h = h;
		sdl.Init( m_w, m_h);
		m_sz = sizeof( *m_arr) * 3 * m_w * m_h;
		m_arr = (double *)malloc( m_sz);
		double t = 0;

#if 0
// origins
		// camera
		v3 cam[] = {
#define ED 3
			{ ED, ED, ED},	// eye
			{ -1, -1, -1},	// front towards screen
			{ 0, 0, 1},		// up along screen
		};
		// scene
		double sph[][CSphere::MAX] = {
#define SR 0.1
			{ 0, 0, 0, SR, 0, 0, 0},
			{ 1, 0, 0, SR, 1, 0, 0},
			{ 0, 1, 0, SR, 0, 1, 0},
			{ 0, 0, 1, SR, 0, 0, 1},
#else
// ioccc ray
		// camera
		v3 cam[] = {
#define ED 3
			{ 0*ED, 0*ED, 1*ED},	// eye
			{ 0, 0, -1},	// front towards screen
			{ 0, 1, 0},		// up along screen
		};
		// scene
		double sph[][CSphere::MAX] = {
#define WINSCALE 1.0
#define SR 0.05
#define R 1.0
#define G 1.0
#define B 1.0
#define Z 0
#define CX 0.05*WINSCALE
#define CY 0.08*WINSCALE
#define CZ 0.1*WINSCALE
    { [CSphere::CENTER_X] = -7*CX, [CSphere::CENTER_Y] = 4*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -3*CX, [CSphere::CENTER_Y] = 4*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 2*CX, [CSphere::CENTER_Y] = 4*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 5*CX, [CSphere::CENTER_Y] = 4*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 8*CX, [CSphere::CENTER_Y] = 4*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },

    { [CSphere::CENTER_X] = -7*CX, [CSphere::CENTER_Y] = 3*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -4*CX, [CSphere::CENTER_Y] = 3*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -2*CX, [CSphere::CENTER_Y] = 3*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 1*CX, [CSphere::CENTER_Y] = 3*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 4*CX, [CSphere::CENTER_Y] = 3*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 7*CX, [CSphere::CENTER_Y] = 3*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },

    { [CSphere::CENTER_X] = -7*CX, [CSphere::CENTER_Y] = 2*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -4*CX, [CSphere::CENTER_Y] = 2*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -2*CX, [CSphere::CENTER_Y] = 2*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 1*CX, [CSphere::CENTER_Y] = 2*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 4*CX, [CSphere::CENTER_Y] = 2*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 7*CX, [CSphere::CENTER_Y] = 2*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },

    { [CSphere::CENTER_X] = -7*CX, [CSphere::CENTER_Y] = 1*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -3*CX, [CSphere::CENTER_Y] = 1*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 2*CX, [CSphere::CENTER_Y] = 1*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1*G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 5*CX, [CSphere::CENTER_Y] = 1*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1*B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 8*CX, [CSphere::CENTER_Y] = 1*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = SR, [CSphere::COLOR_RED] = 1*R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },

#undef CX
#undef CY
#undef CZ
#undef R
#undef G
#undef B
#define R 0.0
#define G 0.0
#define B 0.0
#define CX 0.025*WINSCALE
#define CY 0.013*WINSCALE
#define CZ 0.05*WINSCALE
#undef Z
#define Z 5
    { [CSphere::CENTER_X] = -11*CX, [CSphere::CENTER_Y] = -5*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = 1, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -9*CX, [CSphere::CENTER_Y] = -5*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = 1, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 0*CX, [CSphere::CENTER_Y] = -5*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 7*CX, [CSphere::CENTER_Y] = -5*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 11*CX, [CSphere::CENTER_Y] = -5*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },

    { [CSphere::CENTER_X] = -11*CX, [CSphere::CENTER_Y] = -10*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = 1, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -7*CX, [CSphere::CENTER_Y] = -10*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = 1, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -2*CX, [CSphere::CENTER_Y] = -10*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 2*CX, [CSphere::CENTER_Y] = -10*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 7*CX, [CSphere::CENTER_Y] = -10*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 11*CX, [CSphere::CENTER_Y] = -10*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },

    { [CSphere::CENTER_X] = -11*CX, [CSphere::CENTER_Y] = -15*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = 1, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -9*CX, [CSphere::CENTER_Y] = -15*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = 1, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -2*CX, [CSphere::CENTER_Y] = -15*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 0*CX, [CSphere::CENTER_Y] = -15*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 2*CX, [CSphere::CENTER_Y] = -15*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 9*CX, [CSphere::CENTER_Y] = -15*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },

    { [CSphere::CENTER_X] = -11*CX, [CSphere::CENTER_Y] = -20*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = 1, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -7*CX, [CSphere::CENTER_Y] = -20*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = 1, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = -2*CX, [CSphere::CENTER_Y] = -20*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 2*CX, [CSphere::CENTER_Y] = -20*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = 1, [CSphere::COLOR_BLUE] = B, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
    { [CSphere::CENTER_X] = 9*CX, [CSphere::CENTER_Y] = -20*CY, [CSphere::CENTER_Z] = Z*CZ, [CSphere::RADIUS] = 1*SR, [CSphere::COLOR_RED] = R, [CSphere::COLOR_GREEN] = G, [CSphere::COLOR_BLUE] = 1, [CSphere::A0] = 1.0, [CSphere::B0] = 1.0, [CSphere::C0] = 1.0, [CSphere::D0] = 1.0, [CSphere::E0] = 1.0, [CSphere::F0] = 1.0 },
#endif
		};
		// screen
		m_ww = 1;
		m_hh = m_ww * m_h / m_w;
		printf( "ww=%f hh=%f\n", m_ww, m_hh);
		// eye
#define ED 3
		int i = 0;
		vcopy( m_e, cam[i++]);
		vcopy( m_f, cam[i++]);
		vcopy( m_u, cam[i++]);

		vnorm( m_u);
		vcross( m_r, m_f, m_u);		// compute right
		vcross( m_u, m_r, m_f);		// re-compute up
		vnorm( m_u);
		vnorm( m_r);

		vprint( "e", m_e);
		vprint( "f", m_f);
		vprint( "u", m_u);
		vprint( "r", m_r);
		printf( "\n");

		for (unsigned ii = 0; ii < (sizeof(sph) / sizeof(sph[0])); ii++) {
			m_objs.push_back( new CSphere( sph[ii]));
		}

		memset( m_arr, 0, m_sz);
		int quit = 0;
		while (!quit) {
			int ev = sdl.Poll();
			switch (ev) {
				case CSDL::QUIT:
					quit = 1;
					break;
				case CSDL::LEFT:
					vsub( m_e, m_e, m_r);
					break;
				case CSDL::RIGHT:
					vadd( m_e, m_e, m_r);
					break;
				case CSDL::UP:
					vadd( m_e, m_e, m_u);
					break;
				case CSDL::DOWN:
					vsub( m_e, m_e, m_u);
					break;
			}
			if (quit)
				break;
			Render( t);
			t += 0.1;
			sdl.Draw( m_arr);
			if (ev != CSDL::NONE) {
				sdl.Delay( 50);
			}
			else {
				sdl.Delay( 500);
			}
		}
		free( m_arr);
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			delete m_objs.at( ii);
			m_objs.at( ii) = 0;
		}
	}
private:
	unsigned m_w, m_h;	// screen pixel dimensions
	double *m_arr;
	unsigned m_sz;
	std::vector<CObject *> m_objs;

	v3 m_e;	// eye position
	v3 m_f;	// front towards screen
	v3 m_u;	// up along screen
	v3 m_r;	// right along screen (computed)
	double m_ww, m_hh;	// screen dimensions (space)
};

int main() {
	CRealist r;
	r.Run();
	return 0;
}
