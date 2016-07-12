
#include "CSDL.h"

class CRealist {
public:
	void Render( double t) {
		memset( m_arr, 0, m_sz);
		// eye
//		double ex, ey, ez;
		// screen
//		double sx, sy, sz;
		// ray
//		double x, y, z;
		// color
		double r = 0, g = 0, b = 0;
//		printf( "t=%f\n", t);
				r = (double)((unsigned)t % 256) / 255;
//				printf( " r=%f\n", r);
		for (unsigned jj = 0; jj < m_h; jj++) {
			for (unsigned ii = 0; ii < m_w; ii++) {
//				Trace();
				m_arr[((jj * m_w + ii) * 3) + 0] = r;
				m_arr[((jj * m_w + ii) * 3) + 1] = g;
				m_arr[((jj * m_w + ii) * 3) + 2] = b;
			}
		}
	}
	void Run() {
		CSDL sdl;
		m_w = 100;
		m_h = 100;
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
