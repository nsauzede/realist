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
	CObject():m_hollow(0){
	}
	virtual ~CObject() {
	}
	// returns intersection distance (HUGE_VAL => no intersection)
	virtual double Intersec( const v3& e, const v3& v) const = 0;
	virtual void SetColor( const double *color) {
		m_color = color;
	}
	virtual const v3& Color() const {
		return m_color;
	}
	virtual v3& Center() {
		return m_c;
	}
	virtual const v3& Center() const {
		return m_c;
	}
	virtual void SetHollow( const int& hollow) {
		m_hollow = hollow;
	}
	const int& Hollow() const {
		return m_hollow;
	}
	virtual std::ostream& Serialize( std::ostream& out) const = 0;
	friend std::ostream& operator<<( std::ostream& out, const CObject& ob) {
		return ob.Serialize( out);
	}
protected:
	v3 m_c;
	v3 m_color;
	int m_hollow;
};

typedef class CSphere CLamp;

class CSphere : public CObject {
public:
	enum {
		CENTER_X, CENTER_Y, CENTER_Z, RADIUS, COLOR, COLOR_RED = COLOR, COLOR_GREEN, COLOR_BLUE,
		A0, B0, C0, D0, E0, F0,
		MAX
	};
	const double& Radius() const {
		return m_r;
	}
	virtual std::ostream& Serialize( std::ostream& out) const {
		out << m_c << std::endl;
		out << m_r << std::endl;
		out << m_color << std::endl;
		double v1[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		vec<6> v(v1);
		out << v << std::endl;
		return out;
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
				if (t1 < t2) {
					result = t1;
				} else {
					result = t2;
				}
			}
			else {
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
	int SaveScene( const char *scene_file) const {
		std::ofstream f( scene_file);
		f << m_e << std::endl;
		f << m_f << std::endl;
		f << m_u << std::endl;
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			f << std::endl;
			f << *(m_objs.at( ii)) << std::endl;
		}
		return 0;
	}
	CRealist( const char *scene_file = 0) {
		const char *wfile_name = 0;
		if (scene_file) {
			if (LoadScene( scene_file))
				return;
		}
		else {
// default scene : origins (unit vectors)
			// camera
			v3 cam[] = {
#define ED 3
				{ ED, ED, 1*ED},        // eye
				{ -1, -1, 1*-1},        // front towards screen
				{ 0, 0, 1},             // up along screen
			};
			// scene
			double sph[][CSphere::MAX] = {
#define SR 0.1
				{ 0, 0, 0, SR, 1, 1, 1},
				{ 1, 0, 0, SR, 1, 0, 0},
				{ 0, 1, 0, SR, 0, 1, 0},
				{ 0, 0, 1, SR, 0, 0, 1},
			};

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

		double slamp[] = {
#define LAMP_DIST 0.5
#define LAMP_RAD 0.02
#define LAMP_COL 1.0
			0.0 * LAMP_DIST, -1.0 * LAMP_DIST, 1.0 * LAMP_DIST,
			LAMP_RAD,
			1.0 * LAMP_COL, 1.0 * LAMP_COL, 0.0 * LAMP_COL
		};
		CSphere *lamp = new CLamp( slamp);
		lamp->SetHollow( 1);
		m_lamps.push_back( lamp);
		m_objs.push_back( lamp);

		if (wfile_name) {
			// write scene file here
			SaveScene( wfile_name);
		}
	}
#define MAX_DEPTH 3
	void Trace( int depth, const v3 &o, const v3 &v, v3 &color) const {
		if (depth > MAX_DEPTH)
			return;
#define TMAX 1E10
		double tmin = HUGE_VAL;
		CObject *omin = 0;
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			double tres = m_objs.at( ii)->Intersec( o, v);
			if ((tres > 0) && (tres < tmin)) {
				tmin = tres;
				omin = m_objs.at( ii);
			}
		}
		double def_color = 0;
		color *= def_color;
		if (tmin < TMAX) {
			double energy = 0;
			// ambient
			energy += 0.2;
#if defined USE_FLASH || defined USE_REFL
			// coords of intersec
			v3 vint;
			// normal at intersec
			v3 nv;
#endif
			// intersected object color
			color = omin->Color() * 1.0;
#if defined USE_FLASH || defined USE_REFL
			// coords of intersec
			vint = o + v * tmin;
			// normal at intersec
			nv = ~(vint - omin->Center());
#endif
#ifdef USE_FLASH
			// camera flash
#define MAX_FLASH 0.1
			double flash_nrj = 1 - energy;
			if (flash_nrj > MAX_FLASH)
				flash_nrj = MAX_FLASH;
			double dist = !(vint - m_e);
#define LAMP_FLOOR 0.4
			if (dist < LAMP_FLOOR)
				dist = LAMP_FLOOR;
			energy += flash_nrj / dist / dist;
#endif
			// is there any object intersection between vint and a lamp ?
			for (unsigned jj = 0; jj < m_lamps.size(); jj++) {
				if (omin == m_lamps.at( jj)) // skip current lamp==intersected object
					continue;
				v3 plamp = m_lamps.at( jj)->Center();
				v3 vlamp = plamp - vint;
				// is normal dot vlamp <= 0 (surface not exposed to light)
				if ((vlamp % nv) <= 0)
					continue;
				double dlamp = !vlamp;
				int shadowed = 0;
				for (unsigned ii = 0; ii < m_objs.size(); ii++) {
					if (omin == m_objs.at( ii)) // skip current object=intersected object
						continue;
					double tres = m_objs.at( ii)->Intersec( vint, vlamp);
					if ((tres > 0) && (tres < tmin)) {
						v3 vint2 = vint + vlamp * tres;
						double dint = !(vint2 - vint);
						if ((dint < dlamp) && !m_objs.at( ii)->Hollow()) {
							shadowed = 1;
							break;
						}
					}
				}
				if (!shadowed) {
					double nrj = 0.1 * 1.0 / dlamp / dlamp;
					if (nrj > 1.0)
						nrj = 1.0;
					energy += nrj;
				}
			}
			color *= energy;
#ifdef USE_REFL
			if (!omin->Hollow()) {
				// reflection
				double dot = 2 * (v % nv);
				v3 vrefl = v - nv * dot;
				v3 refl_color = { 0, 0, 0};
				Trace( depth + 1, vint, vrefl, refl_color);
				double refl_att = 0.2;
				color *= (1 - refl_att);
				color += refl_color * refl_att;
			}
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
				v = ~(m_f + vu + vr);
				v3 color = { 1, 1, 1};
				Trace( 0, m_e, v, color);
				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 0] = color[0];
				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 1] = color[1];
				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 2] = color[2];
			}
//			printf( "\n");
		}
	}
#define W 1024
#define H 768
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

		printf( "found %d objects\n", (int)m_objs.size());

		memset( m_arr, 0, m_sz);
		int quit = 0;
		int dirty = 1;
		while (!quit) {
			if (sdl) {
				int ev;
				do {
					ev = sdl->Poll();
					int shift = 0;
					if (ev < 0) {
						ev = -ev;
						shift = 1;
					}
					v3 rv;
					int modif = 1;
					switch (ev) {
						case CSDL::QUIT:
							quit = 1;
							break;
#define LR 0.05
#define UD 0.05
#define PUD 0.1
						case CSDL::LEFT:
							rv = m_r * -LR;
							break;
						case CSDL::RIGHT:
							rv = m_r * +LR;
							break;
						case CSDL::UP:
							rv = m_u * +UD;
							break;
						case CSDL::DOWN:
							rv = m_u * -UD;
							break;
						case CSDL::PUP:
							rv = m_f * +PUD;
							break;
						case CSDL::PDOWN:
							rv = m_f * -PUD;
							break;
						default:
							modif = 0;
							break;
					}
					if (quit)
						break;
					if (modif) {
						if (!shift)
							m_lamps.at( 0)->Center() += rv;
						else
							m_e += rv;
						dirty = 1;
					}
				} while (ev != CSDL::NONE);
				if (quit)
					break;
				if (dirty) {
					Render( t);
					t += 0.1;
					sdl->Draw( m_arr);
					dirty = 0;
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
	std::vector<CLamp *> m_lamps;

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
