
#include <SDL.h>

class CSDL {
public:
	CSDL() :
		m_w(0),
		m_h(0),
		m_bpp(0),
		m_screen(0){
	}
	void Init( int _w = 100, int _h = 100, int _bpp = 32) {
#ifdef SDL1
#define SDLV 1
#else
#define SDLV 2
#endif
//	std::cout << "hello SDL " << SDLV << std::endl;
	SDL_Init( SDL_INIT_VIDEO);

	m_w = _w;
	m_h = _h;
	m_bpp = _bpp;
	m_screen = 0;
#ifdef SDL2
	m_sdlWindow = 0;
	m_sdlRenderer = 0;
	m_sdlTexture = 0;
#endif

#ifdef SDL1
	m_screen = SDL_SetVideoMode( m_w, m_h, m_bpp, 0);
#else
	SDL_CreateWindowAndRenderer( m_w, m_h, 0, &m_sdlWindow, &m_sdlRenderer);
	m_screen = SDL_CreateRGBSurface( 0, m_w, m_h, m_bpp,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF,
                                        0xFF000000);
	m_sdlTexture = SDL_CreateTexture( m_sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_w, m_h);
#endif
	if (!m_screen) {
//		std::cout << "failed to init SDL" << std::endl;
		exit( 1);
	}
	atexit( SDL_Quit);
	}
	enum { NONE, QUIT, LEFT, RIGHT, UP, DOWN};
	int Poll() {
		int result = NONE;
		SDL_Event event;
		while (SDL_PollEvent( &event)) {
			if (event.type == SDL_QUIT) {
				result = QUIT;
				break;
			}
			else if (event.type == SDL_KEYUP) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					result = QUIT;
					break;
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					result = LEFT;
					break;
				}
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					result = RIGHT;
					break;
				}
				else if (event.key.keysym.sym == SDLK_UP) {
					result = UP;
					break;
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					result = DOWN;
					break;
				}
			}
		}
		return result;
	}
	void Draw( double *arr = 0) {
		SDL_Rect rect;
		if (arr) {
			rect.w = 1;
			rect.h = 1;
			double *_arr = arr;
			for (unsigned jj = 0; jj < m_h; jj++) {
				rect.y = jj;
				for (unsigned ii = 0; ii < m_w; ii++) {
					rect.x = ii;
					unsigned r, g, b;
					r = _arr[0] * 255;
					g = _arr[1] * 255;
					b = _arr[2] * 255;
					Uint32 col = SDL_MapRGB( m_screen->format, r, g, b);
					SDL_FillRect( m_screen, &rect, col);
					_arr += 3;
				}
			}
		}
		else {
			rect.x = 0;
			rect.y = 0;
			rect.w = m_w;
			rect.h = m_h;
			Uint32 col = SDL_MapRGB( m_screen->format, 128, 0, 0);
			SDL_FillRect( m_screen, &rect, col);
		}
#ifdef SDL1
		SDL_UpdateRect( m_screen, 0, 0, 0, 0);
#else
		SDL_UpdateTexture( m_sdlTexture, NULL, m_screen->pixels, m_screen->pitch);
		SDL_RenderClear( m_sdlRenderer);
		SDL_RenderCopy( m_sdlRenderer, m_sdlTexture, NULL, NULL);
		SDL_RenderPresent( m_sdlRenderer);
#endif
	}
	void Delay( unsigned millis) {
		SDL_Delay( millis);
	}
private:
	unsigned int m_w;
	unsigned int m_h;
	unsigned int m_bpp;
	SDL_Surface *m_screen;
#ifdef SDL2
	SDL_Window *m_sdlWindow;
	SDL_Renderer *m_sdlRenderer;
	SDL_Texture *m_sdlTexture;
#endif
};
