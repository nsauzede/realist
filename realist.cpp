
#include "CSDL.h"

class CRealist {
public:
	void Render( int val) {
		memset( m_arr, val, m_sz);
	}
	void Run() {
		CSDL sdl;
		m_w = 1024;
		m_h = 768;
		sdl.Init( m_w, m_h);
		m_sz = sizeof( uint8_t) * 3 * m_w * m_h;
		m_arr = (uint8_t*)malloc( m_sz);
		int val = 0;
		while (1) {
			if (sdl.Poll())
				break;
			Render( val++);
			sdl.Draw( m_arr);
			sdl.Delay( 100);
		}
		free( m_arr);
	}
private:
	int m_w, m_h;
	uint8_t *m_arr;
	unsigned m_sz;
};

int main() {
	CRealist r;
	r.Run();
	return 0;
}
