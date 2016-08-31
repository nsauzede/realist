#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include <vector>
#include <iostream>
#include <fstream>

#include "vec.h"

enum { OT_NONE = -1, OT_SPHERE = 0, OT_PLANE };
class CObject {
public:
	CObject( int type = OT_NONE, int len = 0) : m_type( type), m_len( len), m_flags( 0), m_hollow(0) {
	}
	virtual ~CObject() {
	}
	// returns intersection distance (HUGE_VAL => no intersection)
	virtual double Intersec( const v3& e, const v3& v) const = 0;
	virtual v3 Normal( const v3& vint) const = 0;
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
	virtual std::ostream& Serialize( std::ostream& out) const {
		out << m_type << " " << m_len << std::endl;
		return out;
	}
	friend std::ostream& operator<<( std::ostream& out, const CObject& ob) {
		return ob.Serialize( out);
	}
	virtual void Json( std::ostream& out) const = 0;
protected:
	int m_type;
	int m_len;		// for serialization : total number of double excluding type and len
//---------------
	int m_flags;
	v3 m_color;
	int m_hollow;
	v3 m_c;
};

typedef class CSphere CLamp;

class CSphere : public CObject {
public:
	enum {
		FLAGS, COLOR, COLOR_RED = COLOR, COLOR_GREEN, COLOR_BLUE, CENTER, CENTER_X = CENTER, CENTER_Y, CENTER_Z, RADIUS,
//		A0, B0, C0, D0, E0, F0,
		MAX
	};
	CSphere( const double *params) :
		CObject( OT_SPHERE, MAX),
		m_r( params[RADIUS]) {
		m_c = &params[CENTER];
		SetColor( &params[COLOR]);
//		std::cout << *this << std::endl;
	}
	const double& Radius() const {
		return m_r;
	}
	std::ostream& Serialize( std::ostream& out) const {
		CObject::Serialize( out);
		out << m_flags << std::endl;
		out << m_color << std::endl;
		out << m_c << std::endl;
		out << m_r << std::endl;
		return out;
	}
	void Json( std::ostream& out) const {
		out << "{'type':'sphere', 'data': [";
		m_c.Print( out);
		out << ",\t";
		out << m_r;
		out << ",\t";
		m_color.Print( out);
		out << "]}";
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
	v3 Normal( const v3& vint) const {
		v3 nv = ~(vint - Center());
		return nv;
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

class CPlane : public CObject {
public:
	enum {
		FLAGS, COLOR, COLOR_RED = COLOR, COLOR_GREEN, COLOR_BLUE,
		LOC0, LOC0_X = LOC0, LOC0_Y, LOC0_Z,
		LOC1, LOC1_X = LOC1, LOC1_Y, LOC1_Z,
		LOC2, LOC2_X = LOC2, LOC2_Y, LOC2_Z,
		MAX
	};
	CPlane( const double *params) :
		CObject( OT_PLANE, MAX),
		m_loc0( &params[LOC0]),
		m_loc1( &params[LOC1]),
		m_loc2( &params[LOC2])
	{
		SetColor( &params[COLOR]);
		std::cout << *this << std::endl;
	}
	std::ostream& Serialize( std::ostream& out) const {
		CObject::Serialize( out);
		out << m_flags << std::endl;
		out << m_color << std::endl;
		out << m_loc0 << std::endl;
		out << m_loc1 << std::endl;
		out << m_loc2 << std::endl;
		return out;
	}
	void Json( std::ostream& out) const {
		out << "'plane': [";
		m_loc0.Print( out);
		out << ",\t";
		m_loc1.Print( out);
		out << ",\t";
		m_loc2.Print( out);
		out << ",\t";
		m_color.Print( out);
		out << "]";
	}
	double Intersec( const v3 &oi, const v3 &vi) const {
		double res = HUGE_VAL;
		v3 n = Normal( m_loc0);
		v3 p = m_loc0 - oi;
		double num = p % n;
		double den = vi % n;
		if (den == 0) {
			if (num == 0) {
				printf( "all intersections\n");
			}
			else {
				printf( "no intersections\n");
			}
		}
		else {
//			printf( "to be computed\n");
			double det;
			double a, b, c, d, e, f, g, h, i;
			v3 lb = oi + vi;
			a = oi[0] - lb[0];
			b = m_loc1[0] - m_loc0[0];
			c = m_loc2[0] - m_loc0[0];
			d = oi[1] - lb[1];
			e = m_loc1[1] - m_loc0[1];
			f = m_loc2[1] - m_loc0[1];
			g = oi[2] - lb[2];
			h = m_loc1[2] - m_loc0[2];
			i = m_loc2[2] - m_loc0[2];
			det = a * (e * i - f * h) - b * (i * d - f * g) + c * (d * h - e * g);
			if (det > 0.001) {	// matrix is invertible
				double A, B, C, D, E, F, G, H, I;
				A = (e * i - f * h);
				B = -(d * i - f * g);
				C = (d * h - e * g);
				D = -(b * i - c * h);
				E = (a * i - c * g);
				F = -(a * h - b * g);
				G = (b * f - c * e);
				H = -(a * f - c * d);
				I = (a * e - b * d);
				double t, u = 0, v = 0;
				// p=p0+u*v1+v*v2
				t = (A * (oi[0] - m_loc0[0]) + D * (oi[1] - m_loc0[1]) + G * (oi[2] - m_loc0[2])) / det;
				u = (B * (oi[0] - m_loc0[0]) + E * (oi[1] - m_loc0[1]) + H * (oi[2] - m_loc0[2])) /det;
				v = (C * (oi[0] - m_loc0[0]) + F * (oi[1] - m_loc0[1]) + I * (oi[2] - m_loc0[2])) / det;
				if ((u >= 0 && u <= 1) && (v >= 0 && v <= 1) && ((u + v) <= 1)) {
					if (t > 1) {
						res = 1;
					}
				}
			}
		}
		return res;
	}
	v3 Normal( const v3& vint) const {
		v3 v1 = m_loc1 - vint;
		v3 v2 = m_loc2 - vint;
		v3 n;
		n = ~(v1 ^ v2);
		return n;
	}
	friend std::ostream& operator<<( std::ostream& out, const CPlane& pl) {
		out << "plane: loc0=";
		out << pl.m_loc0;
		out << " loc1=";
		out << pl.m_loc1;
		out << " loc2=";
		out << pl.m_loc2;
		out << " col=";
		out << pl.Color();
		return out;
	}
private:
	v3 m_loc0, m_loc1, m_loc2;
};

class CRealist {
public:
	void JsonScene( std::ostream& out) const {
		out << "{" << std::endl;
		out << "'screen': { 'w':" << m_w << ", 'h':" << m_h << ", 'ratiox':1, 'ratioy':1 }," << std::endl;
		out << "'camera': { ";
		out << "'loc':";
		m_e.Json( out);
		out << ", 'front':";
		m_f.Json( out);
		out << ", 'up':";
		m_u.Json( out);
		out << "}," << std::endl;
		out << "'objects': [" << std::endl;
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			if (ii > 0)
				out << "," << std::endl;
			(m_objs.at( ii))->Json( out);
		}
		out << std::endl << "]" << std::endl;
		out << "}" << std::endl;
	}
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
			unsigned type;
			unsigned len;
			ifs >> type;
			if (ifs.eof())
				break;
			result = -1;
			ifs >> len;
//			printf( "read type=%u len=%u\n", type, len);
			double *data = new double[len];
			for (unsigned ii = 0; ii < len; ii++) {
				if (ifs.eof())
					break;
				ifs >> data[ii];
			}
			if (ifs.eof())
				break;
			CObject *obj = 0;
			switch (type) {
				case OT_SPHERE:
					obj = new CSphere( data);
					break;
			}
			if (obj)
				m_objs.push_back( obj);
			delete data;
			result = 0;
		}
//		JsonScene( std::cout);
		return result;
	}
	void OutScene( std::ostream& out) const {
		out << m_e << std::endl;
		out << m_f << std::endl;
		out << m_u << std::endl;
		out << std::endl;
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			out << *(m_objs.at( ii)) << std::endl;
		}
	}
	int SaveScene( const char *scene_file) const {
		std::ofstream f( scene_file);
		OutScene( f);
		return 0;
	}
#define W 1024
#define H 768
	CRealist( const char *scene_file = 0):
		m_w(W),
		m_h(H) {
//		std::cout << "# initial #objects: " << m_objs.size() << std::endl;
		const char *wfile_name = 0;
		if (scene_file) {
			if (LoadScene( scene_file)) {
				printf( "failed to load\n");
				exit( 1);
			}
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
				{ 0, 1, 1, 1, 0, 0, 0, SR},
				{ 0, 1, 0, 0, 1, 0, 0, SR},
				{ 0, 0, 1, 0, 0, 1, 0, SR},
				{ 0, 0, 0, 1, 0, 0, 1, SR},
			};

			int i = 0;
			m_e = cam[i++];
			m_f = cam[i++];
			m_u = cam[i++];

			for (unsigned ii = 0; ii < (sizeof(sph) / sizeof(sph[0])); ii++) {
				m_objs.push_back( new CSphere( sph[ii]));
			}
			double pl[][CPlane::MAX] = {
			{
				0.0, 0.0, 0.0,
				1.0, 0.0, 0.0,
				0.0, 0.0, 1.0,
				1.0, 0.0, 0.0,
			},
			{
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0,
				0.0, 0.0, 1.0,
				0.0, 1.0, 0.0,
			},
			{
				0.0, 0.0, 0.0,
				1.0, 0.0, 0.0,
				0.0, 1.0, 0.0,
				0.0, 0.0, 1.0,
			},
			};
			for (unsigned ii = 0; ii < (sizeof(pl) / sizeof(pl[0])); ii++) {
				m_objs.push_back( new CPlane( pl[ii]));
			}
		}
		if (m_r[0] == 0 && m_r[1] == 0 && m_r[2] == 0) {
			m_u = ~m_u;
			m_r = m_f ^ m_u;		// compute right
			m_u = m_r ^ m_f;		// re-compute up
			m_u = ~m_u;
			m_r = ~m_r;
		}

		double slamp[] = {
			0,
#define LAMP_DIST 0.5
#define LAMP_RAD 0.02
#define LAMP_COL 1.0
			1.0 * LAMP_COL, 1.0 * LAMP_COL, 0.0 * LAMP_COL,
			0.0 * LAMP_DIST, -1.0 * LAMP_DIST, 1.0 * LAMP_DIST,
			LAMP_RAD,
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
//		unsigned imin = 0;
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			double tres = m_objs.at( ii)->Intersec( o, v);
			if ((tres > 0) && (tres < tmin)) {
				tmin = tres;
				omin = m_objs.at( ii);
//				imin = ii;
			}
		}
		double def_color = 0;
		color *= def_color;
		if (tmin < TMAX) {
			double energy = 0;
			// ambient
#if defined USE_FLASH || defined USE_REFL || defined USE_LAMP
			energy += 0.2;
			v3 vint;
			v3 nv;
#else
			energy += 1.0;
#endif
//			printf( "[HIT %u]", imin);
			// intersected object color
			color = omin->Color() * 1.0;
#if defined USE_FLASH || defined USE_REFL || defined USE_LAMP
			// coords of intersec
			vint = o + v * tmin;
			// normal at intersec
			nv = ~omin->Normal( vint);
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
#ifdef USE_LAMP
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
#endif
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
	void Render( unsigned w = 0, unsigned h = 0, char *fnameout = 0) {
		if (w && h) {
			m_w = w;
			m_h = h;
		}
//		m_sz = sizeof( *m_arr) * 3 * m_w * m_h;
//		m_arr = (double *)malloc( m_sz);

		// screen
		m_ww = 1;
		m_hh = m_ww * m_h / m_w;
		FILE *fout = stdout;
		if (fnameout) {
			fout = fopen( fnameout, "wb");
		}
		fprintf( fout, "P3\n");
		fprintf( fout, "%d %d\n", m_w, m_h);
		fprintf( fout, "%d\n", 100);

//		memset( m_arr, 0, m_sz);
		// ray
		v3 v;
//		printf( "tr=%f\n", tr);
		for (unsigned jj = 0; jj < m_h; jj++) {
			v3 vu;
			vu = m_u * ((double)m_h - (double)jj - 1.0 - (double)m_h / 2) / (double)m_h * m_hh;
//			vprint( "raycpp vu", vu);
//			getchar();
			for (unsigned ii = 0; ii < m_w; ii++) {
				v3 vr;
				vr = m_r * ((double)ii - m_w / 2) / m_w * m_ww;
				v = ~(m_f + vu + vr);
				v3 color = { 1, 1, 1};
				Trace( 0, m_e, v, color);
//				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 0] = color[0];
//				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 1] = color[1];
//				m_arr[(((m_h - jj - 1) * m_w + ii) * 3) + 2] = color[2];
				fprintf( fout, "%2.lf %2.lf %2.lf   ", 100*color[0], 100*color[1], 100*color[2]);
			}
			fprintf( fout, "\n");
		}
		if (fout != 0 && fout != stdout) {
			fflush( fout);
			fclose( fout);
		}
//		free( m_arr);
		for (unsigned ii = 0; ii < m_objs.size(); ii++) {
			delete m_objs.at( ii);
			m_objs.at( ii) = 0;
		}
	}
private:
	unsigned m_w, m_h;	// screen pixel dimensions
//	double *m_arr;
//	unsigned m_sz;
	std::vector<CObject *> m_objs;
	std::vector<CLamp *> m_lamps;

	v3 m_e;	// eye position
	v3 m_f;	// front towards screen
	v3 m_u;	// up along screen
	v3 m_r;	// right along screen (computed)
	double m_ww, m_hh;	// screen dimensions (space)
};

int main( int argc, char *argv[]) {
	unsigned w = 0, h = 0;
	char *fnameout = 0;
	char *scene = 0;
	int arg = 1;
	if (arg < argc) {
		scene = argv[arg++];
		if (arg < argc) {
			sscanf( argv[arg++], "%d", &w);
			if (arg < argc) {
				sscanf( argv[arg++], "%d", &h);
				if (arg < argc) {
					fnameout = argv[arg++];
				}
			}
		}
	}
	CRealist r( scene);
	r.Render( w, h, fnameout);
	return 0;
}
