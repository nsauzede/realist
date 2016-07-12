
#include "CSDL.h"

class CRealist {
public:
	void Run() {
		CSDL sdl;
		m_w = 320;
		m_h = 200;
		sdl.Init( m_w, m_h);
		unsigned sz = sizeof( uint8_t) * 3 * m_w * m_h;
		uint8_t *arr = (uint8_t*)malloc( sz);
		int val = 0;
		while (1) {
			memset( arr, val++, sz);
			if (sdl.Poll())
				break;
//			Render();
			sdl.Draw( arr);
			sdl.Delay( 100);
		}
		free( arr);
	}
private:
	int m_w, m_h;
};

int main() {
	CRealist r;
	r.Run();
	return 0;
}
