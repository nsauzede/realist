TARGET=realist

CXXFLAGS=-Wall -Werror
CXXFLAGS+=-Wextra

CXXFLAGS+=-std=c++11

CXXFLAGS+=-g -O0

CXXFLAGS+=-I.

include sdl.mak
ifeq ($(SDL_VER),1)
CXXFLAGS+=-DSDL1
else
CXXFLAGS+=-DSDL2
endif

CXXFLAGS+=$(SDL_FLAGS)
LDLIBS+=$(SDL_LIBS)

all: SDL_CHECK $(TARGET)

clean:
	$(RM) $(TARGET)

clobber: clean
