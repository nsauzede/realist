#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <vector>
#include <iostream>
#include <fstream>

#include "CSDL.h"

#include "vec.h"

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
		m_color = color;
	}
	virtual const v3 &Color() const {
		return m_color;
	}
	virtual const v3 &Center() const {
		return m_c;
	}
private:
	v3 m_color;
protected:
	v3 m_c;
};

class CSphere : public CObject {
public:
	// { center_x, center_y, center_z, radius, color_r, color_g, color_b }
	enum {
		CENTER_X, CENTER_Y, CENTER_Z, RADIUS, COLOR, COLOR_RED = COLOR, COLOR_GREEN, COLOR_BLUE,
		A0, B0, C0, D0, E0, F0,
		MAX
	};
	const double& Radius() const {
		return m_r;
	}
	CSphere( const double *params) :
		m_r( params[RADIUS]) {
		m_c = params;
		SetColor( &params[COLOR]);
	}
	double Intersec( const v3 &e, const v3 &v) const {
		double result = HUGE_VAL;
		double sr2 = m_r * m_r;
		double a, b, c;
		v3 t = e - m_c;
		a = v % v;
		b = 2 * (v % t);
		c = (t % t) - sr2;
		double t1 = 0, t2 = 0;
		int sol = solvetri( a, b, c, t1, t2);
		if (sol >= 1) {
			if (sol > 1) {
//				printf( "t1=%f t2=%f => ", t1, t2);
				if (t1 < t2) {
					result = t1;
				} else {
					result = t2;
				}
			}
			else {
//				printf( "t1=%f => ", t1);
				result = t1;
			}
		}
		return result;
	}
	friend std::ostream& operator<<( std::ostream& out, const CSphere& sp) {
		out << "sphere: c=";
		out << sp.Center();
		out << " r=";
		out << sp.Radius();
		out << " col=";
		out << sp.Color();
		return out;
	}
private:
	double m_r;
};

class CRealist {
public:
	int LoadScene( const char *scene_file) {
		int result = -1;
		std::ifstream ifs( scene_file);
		if (!ifs.is_open())
			return result;
		for (unsigned ii = 0; ii < 3; ii++) {
			ifs >> m_e[ii];
		}
		for (unsigned ii = 0; ii < 3; ii++) {
			ifs >> m_f[ii];
		}
		for (unsigned ii = 0; ii < 3; ii++) {
			ifs >> m_u[ii];
		}
		while (!ifs.eof()) {
			double sp[CSphere::MAX];
			for (unsigned ii = 0; ii < CSphere::MAX; ii++) {
				ifs >> sp[ii];
			}
			CSphere *sph = new CSphere( sp);
			std::cout << *sph << std::endl;
			m_objs.push_back( sph);
		}
		return 0;
	}
	CRealist( const char *scene_file = 0) {
		if (scene_file) {
			if (LoadScene( scene_file))
				return;
		}
		else {
#if 1
// spheres pyramid
		v3 cam[] = {
#define ED 0.4
			{ 1*ED, 0*ED, 1*ED},	// eye
			{ -1, 0, -1},	// front towards screen
			{ 0, 0, 1},		// up along screen
		};
		// scene
		double sph[][CSphere::MAX] = {
#define SR 0.05
#define SD (2*SR)
#define SC 0.8
			{ 0*SD, -1*SD, 0*SR, SR, 1*SC, 1*SC, 1*SC},
			{ 0*SD, +0*SD, 0*SR, SR, 1*SC, 1*SC, 1*SC},
			{ 0*SD, +1*SD, 0*SR, SR, 1*SC, 1*SC, 1*SC},

			{ 1*SD, -1*SR, 0*SR, SR, 1*SC, 0*SC, 0*SC},
			{ 1*SD, +1*SR, 0*SR, SR, 0*SC, 0*SC, 1*SC},

			{ 2*SD, +0*SD, 0*SR, SR, 0*SC, 1*SC, 0*SC},

			{ 1*SR, -1*SR, 1*SD, SR, 1*SC, 1*SC, 1*SC},
			{ 1*SR, +1*SR, 1*SD, SR, 1*SC, 1*SC, 1*SC},
		};
#elif 1
// origins
		// camera
		v3 cam[] = {
#define ED 3
			{ ED, ED, 1*ED},	// eye
			{ -1, -1, 1*-1},	// front towards screen
			{ 0, 0, 1},		// up along screen
		};
		// scene
		double sph[][CSphere::MAX] = {
#define SR 0.1
			{ 0, 0, 0, SR, 0, 0, 0},
#if 1
			{ 1, 0, 0, SR, 1, 0, 0},
			{ 0, 1, 0, SR, 0, 1, 0},
			{ 0, 0, 1, SR, 0, 0, 1},
#endif
		};
#else
// ioccc ray
		// camera
		v3 cam[] = {
#define ED 1.0
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
		};
#endif

		// eye
		int i = 0;
		m_e = cam[i++];
		m_f = cam[i++];
		m_u = cam[i++];

		for (unsigned ii = 0; ii < (sizeof(sph) / sizeof(sph[0])); ii++) {
			m_objs.push_back( new CSphere( sph[ii]));
		}
		}
		if (m_r[0] == 0 && m_r[1] == 0 && m_r[2] == 0) {
			m_u = ~m_u;
			m_r = m_f ^ m_u;		// compute right
			m_u = m_r ^ m_f;		// re-compute up
			m_u = ~m_u;
			m_r = ~m_r;
		}

		vprint( "e", m_e);
		vprint( "f", m_f);
		vprint( "u", m_u);
		vprint( "r", m_r);
		printf( "\n");

	}
#define MAX_DEPTH 2
	void Trace( int depth, const v3 &o, const v3 &v, v3 &color) const {
		if (depth > MAX_DEPTH)
			return;
#define TMAX 1E10
		double tmin = HUGE_VAL;
		CObject *omin = 0;
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			double tres = m_objs.at( ii)->Intersec( o, v);
			if (tres < tmin) {
				tmin = tres;
				omin = m_objs.at( ii);
			}
		}
		double def_color = 0;
		color *= def_color;
		if (tmin < TMAX) {
			double energy = 0;
			// ambient
			energy += 0.5;
#if defined USE_FLASH || defined USE_REFL
			// coords of intersec
			v3 vint;
			// normal at intersec
			v3 nv;
#endif
			// intersected object color
			color = omin->Color();
#if defined USE_FLASH || defined USE_REFL
			// coords of intersec
			vint = o + v * tmin;
			// normal at intersec
			nv = ~(vint - omin->Center());
#endif
#ifdef USE_FLASH
			// camera flash
			energy += nv[2] * (1 - energy);
#endif
			color *= energy;
#ifdef USE_REFL
			// reflection
			double dot = (v % nv);
			v3 vrefl = v + nv * dot;
			v3 refl_color = { 0, 0, 0};
			Trace( depth + 1, vint, vrefl, refl_color);
			double refl_att = 0.1;
			color *= (1 - refl_att);
			color += refl_color * refl_att;
#endif
		}
	}
	void Render( const double &tr = 0) const {
		// ray
		v3 v;
		printf( "tr=%f\n", tr);
		for (unsigned jj = 0; jj < m_h; jj++) {
			v3 vu;
			vu = m_u * ((double)jj - m_h / 2) / m_h * m_hh;
			for (unsigned ii = 0; ii < m_w; ii++) {
				v3 vr;
				vr = m_r * ((double)ii - m_w / 2) / m_w * m_ww;
				v = ~m_f + vu + vr;
				v3 color = { 1, 1, 1};
				Trace( 0, m_e, v, color);
				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 0] = color[0];
				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 1] = color[1];
				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 2] = color[2];
			}
//			printf( "\n");
		}
	}
#if 0
#define W 32
#define H 16
#elif 0
#define W 200
#define H 100
#elif 0
#define W 320
#define H 200
#elif 0
#define W 640
#define H 480
#else
#define W 1024
#define H 768
#endif
	void Run( int nosdl = 0, unsigned w = W, unsigned h = H) {
#ifdef USE_OPT
		printf( "using OPT\n");
#else
		printf( "*NOT* using OPT\n");
#endif
		CSDL *sdl = 0;
		if (!nosdl)
			sdl = new CSDL;
		m_w = w;
		m_h = h;
		if (sdl)
			sdl->Init( m_w, m_h);
		m_sz = sizeof( *m_arr) * 3 * m_w * m_h;
		m_arr = (double *)malloc( m_sz);
		double t = 0;

		// screen
		m_ww = 1;
		m_hh = m_ww * m_h / m_w;
		printf( "ww=%f hh=%f\n", m_ww, m_hh);

		printf( "found %zd objects\n", m_objs.size());

		memset( m_arr, 0, m_sz);
		int quit = 0;
		while (!quit) {
			Render( t);
			t += 0.1;
			if (sdl) {
				int ev = sdl->Poll();
				switch (ev) {
					case CSDL::QUIT:
						quit = 1;
						break;
#define LR 0.05
#define UD 0.05
#define PUD 0.1
					case CSDL::LEFT:
						m_e -= m_r * LR;
						break;
					case CSDL::RIGHT:
						m_e += m_r * LR;
						break;
					case CSDL::UP:
						m_e += m_u * UD;
						break;
					case CSDL::DOWN:
						m_e -= m_u * UD;
						break;
					case CSDL::PUP:
						m_e += m_f * PUD;
						break;
					case CSDL::PDOWN:
						m_e -= m_f * PUD;
						break;
				}
				sdl->Draw( m_arr);
				if (ev == CSDL::NONE) {
					sdl->Delay( 500);
				}
			}
			else
				break;
		}
		free( m_arr);
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			delete m_objs.at( ii);
			m_objs.at( ii) = 0;
		}
		if (sdl) {
			free( sdl);
			sdl = 0;
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

int main( int argc, char *argv[]) {
	int nosdl = 0;
	char *scene = 0;
	int arg = 1;
	if (arg < argc) {
		scene = argv[arg++];
		if (arg < argc) {
			sscanf( argv[arg++], "%d", &nosdl);
		}
	}
	CRealist r( scene);
	r.Run( nosdl);
	return 0;
}
