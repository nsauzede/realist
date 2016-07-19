TARGET=realist

CXXFLAGS=-Wall -Werror
CXXFLAGS+=-Wextra

CXXFLAGS+=-std=c++11

CXXFLAGS+=-g

#USE_OPT=1
ifdef USE_OPT
CXXFLAGS+=-O3
CXXFLAGS+=-DUSE_OPT
else
CXXFLAGS+=-O0
endif

USE_VEC=1
ifdef USE_VEC
CXXFLAGS+=-DUSE_VEC
endif

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
