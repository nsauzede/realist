#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <vector>
#include <iostream>

#include "CSDL.h"

#define DO_REFL

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

#ifdef USE_VEC
#if 0
template<int n> class vec;
typedef vec<3> v3;
template<int n> class vec {
#else
class vec;
typedef vec v3;
class vec {
private:
	static const int n = 3;
#endif
public:
	vec( double d1 = 0, double d2 = 0, double d3 = 0) {
		m_d[0] = d1;
		m_d[1] = d2;
		m_d[2] = d3;
	}
	vec( const double *d) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] = *d++;
		}
	}
	const double& at( unsigned i) const {
		assert( (i < n));
		return m_d[i];
	}
	vec operator^( const vec& r) const {
		vec res;
		res.m_d[0] = this->m_d[1] * r.m_d[2] - this->m_d[2] * r.m_d[1];
		res.m_d[1] = this->m_d[2] * r.m_d[0] - this->m_d[0] * r.m_d[2];
		res.m_d[2] = this->m_d[0] * r.m_d[1] - this->m_d[1] * r.m_d[0];
		return res;
	}
	const vec& operator/=( const double& r) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] /= r;
		}
		return *this;
	}
	vec operator/( const double& r) const {
		vec res = *this;
		res /= r;
		return res;
	}
	const vec& operator*=( const double& r) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] *= r;
		}
		return *this;
	}
	vec operator*( const double& r) const {
		vec res = *this;
		res *= r;
		return res;
	}
	const vec& operator+=( const vec& r) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] += r.m_d[ii];
		}
		return *this;
	}
	vec operator+( const vec& r) const {
		vec res = *this;
		res += r;
		return res;
	}
	const vec& operator-=( const vec& r) {
		for (unsigned ii = 0; ii < n; ii++) {
			m_d[ii] -= r.m_d[ii];
		}
		return *this;
	}
	vec operator-( const vec& r) const {
		vec res = *this;
		res -= r;
		return res;
	}
	double operator!() const {
		return sqrt( *this % *this);
	}
	const double& operator[]( unsigned i) const {
		assert( (i < n));
		return m_d[i];
	}
	vec operator~() const {
		vec res = *this;
		res /= !res;
		return res;
	}
	double operator%( const vec& r) const {
		double result = 0;
		for (unsigned ii = 0; ii < n; ii++) {
			result += m_d[ii] * r.m_d[ii];
		}
		return result;
	}
	void Print() const {
		for (unsigned ii = 0; ii < n; ii++) {
			if (ii > 0)
				printf( ",");
			printf( "%f", m_d[ii]);
		}
		printf( "\n");
	}
private:
	double m_d[n];
};
#else
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

double vdot( const v3 &v1, const v3 &v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vnorm( v3 &l) {
	double norm;
	norm = sqrt( vdot( l, l));
	l[0] /= norm;
	l[1] /= norm;
	l[2] /= norm;
}

#endif

void vprint( const v3 &v) {
	printf( "%f,%f,%f\n", v[0], v[1], v[2]);
}

void vprint( const char *t, const v3 &v) {
	printf( "%s={%f,%f,%f}\n", t, v[0], v[1], v[2]);
}

class CObject {
public:
	virtual ~CObject() {
	}
	// returns intersection distance (HUGE_VAL => no intersection)
	virtual double Intersec( const v3 &e, const v3 &v) const = 0;
	virtual void SetColor( const double *color) {
#ifdef USE_VEC
		m_color = color;
#else
		vcopy( m_color, color);
#endif
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
	CSphere( const double *params) :
		m_r( params[RADIUS]) {
#ifdef USE_VEC
		m_c = params;
#else
		vcopy( m_c, params);
#endif
		SetColor( &params[COLOR]);
	}
	double Intersec( const v3 &e, const v3 &v) const {
		double result = HUGE_VAL;
		double sr2 = m_r * m_r;
		double a, b, c;
#ifdef USE_VEC
		v3 t = e - m_c;
		a = v % v;
		b = 2 * (v % t);
		c = (t % t) - sr2;
#else
		v3 t;
		vsub( t, e, m_c);
		a = vdot( v, v);
		b = 2 * vdot( v, t);
		c = vdot( t, t) - sr2;
#endif
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
private:
	double m_r;
};

class CRealist {
public:
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
#ifdef USE_VEC
		color *= 0.2;
#else
		vmult( color, color, 0.2);
#endif
		if (tmin < TMAX) {
#ifdef USE_VEC
			// intersected object color
			color = omin->Color();
			// coords of intersec
			v3 vint = o + v * tmin;
//			vprint( vint);
//			exit(0);
#ifdef DO_REFL
			// normal at intersec
			v3 nv = ~(vint - omin->Center());
			// reflection
			v3 vrefl = nv * -((v * -1) % nv);
#if 1
			vrefl -= v;
			// refl index
			double index = 0.01;
			vrefl *= 1.0 - index;
			vrefl += v;
#endif
			vrefl = ~vrefl;
#endif
#else
			// intersected object color
			vcopy( color, omin->Color());
			// coords of intersec
			v3 vint;
			vmult( vint, v, tmin);
			vadd( vint, o, vint);
//			vprint( vint);
#ifdef DO_REFL
			// normal at intersec
			v3 nv;
			vsub( nv, vint, omin->Center());
			vnorm( nv);
			// reflection
			v3 vrefl;
			v3 mv;
			vmult( mv, v, -1);
			double dot = vdot( mv, nv);
			vmult( vrefl, nv, -dot);
#if 1
			vsub( vrefl, vrefl, v);
			// refl index
			double index = 0.01;
			vmult( vrefl, vrefl, 1.0 - index);
			vadd( vrefl, vrefl, v);
#endif
			vnorm( vrefl);
#endif
#endif
#ifdef DO_REFL
			v3 refl_color = { 0, 0, 0};
			Trace( depth + 1, vint, vrefl, refl_color);
#ifdef USE_VEC
			refl_color *= 0.5;
			color = ~(color + refl_color);
#else
			vmult( refl_color, refl_color, 0.5);
			vadd( color, color, refl_color);
			vnorm( color);
#endif
#endif
		}
	}
	void Render( const double &tr = 0) const {
		// ray
		v3 v;
		printf( "tr=%f\n", tr);
		for (unsigned jj = 0; jj < m_h; jj++) {
			v3 vu;
#ifdef USE_VEC
			vu = m_u * ((double)jj - m_h / 2) / m_h * m_hh;
#else
			vmult( vu, m_u, ((double)jj - m_h / 2) / m_h * m_hh);
#endif
			for (unsigned ii = 0; ii < m_w; ii++) {
				v3 vr;
#ifdef USE_VEC
				vr = m_r * ((double)ii - m_w / 2) / m_w * m_ww;
				v = ~m_f + vu + vr;
#else
				vmult( vr, m_r, ((double)ii - m_w / 2) / m_w * m_ww);
				vcopy( v, m_f);
				vnorm( v);
				vadd( v, v, vu);
				vadd( v, v, vr);
#endif
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
#ifdef USE_VEC
		printf( "using VEC\n");
#else
		printf( "*NOT* using VEC\n");
#endif
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

#if 0
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
#endif
		};
		// screen
		m_ww = 1;
		m_hh = m_ww * m_h / m_w;
		printf( "ww=%f hh=%f\n", m_ww, m_hh);
		// eye
		int i = 0;
#ifdef USE_VEC
		m_e = cam[i++];
		m_f = cam[i++];
		m_u = cam[i++];

		m_u = ~m_u;
		m_r = m_f ^ m_u;		// compute right
		m_u = m_r ^ m_f;		// re-compute up
		m_u = ~m_u;
		m_r = ~m_r;
#else
		vcopy( m_e, cam[i++]);
		vcopy( m_f, cam[i++]);
		vcopy( m_u, cam[i++]);

		vnorm( m_u);
		vcross( m_r, m_f, m_u);		// compute right
		vcross( m_u, m_r, m_f);		// re-compute up
		vnorm( m_u);
		vnorm( m_r);
#endif

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
			Render( t);
			t += 0.1;
			if (sdl) {
				int ev = sdl->Poll();
#ifndef USE_VEC
				v3 v;
#endif
				switch (ev) {
					case CSDL::QUIT:
						quit = 1;
						break;
#ifdef USE_VEC
					case CSDL::LEFT:
						m_e -= m_r * 0.1;
						break;
					case CSDL::RIGHT:
						m_e += m_r * 0.1;
						break;
					case CSDL::UP:
						m_e += m_u * 0.1;
						break;
					case CSDL::DOWN:
						m_e -= m_u * 0.1;
						break;
#else
					case CSDL::LEFT:
						vcopy( v, m_r);
						vmult( v, v, 0.1);
						vsub( m_e, m_e, v);
						break;
					case CSDL::RIGHT:
						vcopy( v, m_r);
						vmult( v, v, 0.1);
						vadd( m_e, m_e, v);
						break;
					case CSDL::UP:
						vcopy( v, m_u);
						vmult( v, v, 0.1);
						vadd( m_e, m_e, v);
						break;
					case CSDL::DOWN:
						vcopy( v, m_u);
						vmult( v, v, 0.1);
						vsub( m_e, m_e, v);
						break;
#endif
				}
				sdl->Draw( m_arr);
				if (ev != CSDL::NONE) {
					sdl->Delay( 50);
				}
				else {
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
	int arg = 1;
	if (arg < argc) {
		sscanf( argv[arg++], "%d", &nosdl);
	}
	CRealist r;
	r.Run( nosdl);
	return 0;
}
