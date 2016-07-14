
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

void vmult( v3 &l, const v3 &r1, const double &r2) {
	l[0] = r1[0] + r2;
	l[1] = r1[1] + r2;
	l[2] = r1[2] + r2;
}

void vprint( const char *t, const v3 &v) {
	printf( "%s={%f,%f,%f}\n", t, v[0], v[1], v[2]);
}

class CRealist {
public:
	void Trace( /*const double &t,*/ const v3 &e, const v3 &v, v3 &color) {
//		printf( "t=%f\n", t);
//		vprint( e);
//		vprint( v);
		v3 va;
		vadd( va, e, v);
		vprint( "e+v", va);
		vmult( color, color, 0);
//		printf( "\n");
	}
	void Render( double tr) {
		memset( m_arr, 0, m_sz);
		// eye
		v3 e = { 3, 3, 0};
		v3 f = { -1, -1, 0};	// front towards screen
		v3 u = { 0, 0, 1};		// up along screen
		v3 r;
		vcross( r, f, u);			// right along screen
		// ray
//		double x, y, z;
		v3 v;
		// color
		v3 color = { 0, 0, 0};
		printf( "tr=%f\n", tr);
		vprint( "e", e);
		vprint( "f", f);
		vprint( "u", u);
		vprint( "r", r);
		for (unsigned jj = 0; jj < m_h; jj++) {
			v3 vu;
			vmult( vu, u, jj);
			for (unsigned ii = 0; ii < m_w; ii++) {
				v3 vr;
				vmult( vr, r, ii);
				vadd( v, e, f);
				vadd( v, v, vu);
				vadd( v, v, vr);
				Trace( /*tr,*/ e, v, color);
				m_arr[((jj * m_w + ii) * 3) + 0] = color[0];
				m_arr[((jj * m_w + ii) * 3) + 1] = color[1];
				m_arr[((jj * m_w + ii) * 3) + 2] = color[2];
			}
			printf( "\n");
		}
	}
	void Run() {
		CSDL sdl;
		m_w = 6;
		m_h = 6;
		sdl.Init( m_w, m_h);
		m_sz = sizeof( *m_arr) * 3 * m_w * m_h;
		m_arr = (double *)malloc( m_sz);
		double t = 0;
		while (1) {
			if (sdl.Poll())
				break;
			Render( t);
			t += 1.0;
			sdl.Draw( m_arr);
			sdl.Delay( 100);
		}
		free( m_arr);
	}
private:
	unsigned m_w, m_h;
	double *m_arr;
	unsigned m_sz;
};

int main() {
	CRealist r;
	r.Run();
	return 0;
}
