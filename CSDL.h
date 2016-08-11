#include <stdio.h>

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
	m_screen = SDL_SetVideoMode( m_w, m_h, m_bpp, 0
//	|| SDL_FULLSCREEN
	);
	SDL_EnableKeyRepeat( 1+0*SDL_DEFAULT_REPEAT_DELAY, 1+0*SDL_DEFAULT_REPEAT_INTERVAL);
#else
	SDL_CreateWindowAndRenderer( m_w, m_h, 0
//	|| SDL_WINDOW_FULLSCREEN_DESKTOP
	, &m_sdlWindow, &m_sdlRenderer);
	m_screen = SDL_CreateRGBSurface( 0, m_w, m_h, m_bpp,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF,
                                        0xFF000000);
	m_sdlTexture = SDL_CreateTexture( m_sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_w, m_h);
#endif
	if (!m_screen) {
		printf( "failed to init SDL\n");
		exit( 1);
	}
	atexit( SDL_Quit);
	}
	enum { NONE, QUIT, LEFT, RIGHT, UP, DOWN, PUP, PDOWN, K_d, K_j};
	int Poll( int *_ctrl = 0, int *_shift = 0) {
		int result = NONE;
		SDL_Event event;
		int ctrl = 0;
		int shift = 0;
		while (SDL_PollEvent( &event)) {
			if (event.type == SDL_QUIT) {
				result = QUIT;
				break;
			}
#ifdef SDL1
#if 0
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button == SDL_BUTTON_WHEELUP)
					result = PUP;
				else if (event.button == SDL_BUTTON_WHEELUP)
					result = PDOWN;
			}
#endif
#else
			else if (event.type == SDL_MOUSEWHEEL) {
				int deltax = 0;
				int deltay = 0;
				static int last_x = -1;
				int x = event.wheel.x;
				static int last_y = -1;
				int y = event.wheel.y;
				if (last_x != -1) {
					deltax = x - last_x;
				}
				if (last_y != -1) {
					deltay = y - last_y;
				}
				last_x = x;
				last_y = y;
				if (deltax < 0)
					result = LEFT;
				else if (deltax > 0)
					result = RIGHT;
				else if (deltay < 0)
					result = UP;
				else if (deltay > 0)
					result = DOWN;
			}
#endif
			else if (event.type == SDL_MOUSEMOTION) {
				int deltax = 0;
				int deltay = 0;
				static int last_x = -1;
				int x = event.motion.x;
				static int last_y = -1;
				int y = event.motion.y;
				if (last_x != -1) {
					deltax = x - last_x;
				}
				if (last_y != -1) {
					deltay = y - last_y;
				}
				last_x = x;
				last_y = y;
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					if (deltax < 0)
						result = LEFT;
					else if (deltax > 0)
						result = RIGHT;
					else if (deltay < 0)
						result = UP;
					else if (deltay > 0)
						result = DOWN;
				}
			}
			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.mod & KMOD_CTRL)
					ctrl = 1;
				if (event.key.keysym.mod & KMOD_SHIFT)
					shift = 1;
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
				else if (event.key.keysym.sym == SDLK_PAGEUP) {
					result = PUP;
					break;
				}
				else if (event.key.keysym.sym == SDLK_PAGEDOWN) {
					result = PDOWN;
					break;
				}
				else if (event.key.keysym.sym == SDLK_d) {
					result = K_d;
					break;
				}
				else if (event.key.keysym.sym == SDLK_j) {
					result = K_j;
					break;
				}
			}
		}
		if (_ctrl)
			*_ctrl = ctrl;
		if (_shift)
			*_shift = shift;
		if (result != NONE) {
//			printf( "ctrl=%d shift=%d\n", ctrl, shift);
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
