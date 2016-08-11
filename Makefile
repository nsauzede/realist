TARGET=realist

CXXFLAGS=-Wall -Werror
CXXFLAGS+=-Wextra

CXXFLAGS+=-std=c++11

CXXFLAGS+=-g

CXXFLAGS+=-pipe
LDFLAGS+=-pipe

USE_OPT=1
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

#USE_FLASH=1
ifdef USE_FLASH
CXXFLAGS+=-DUSE_FLASH
endif

USE_REFL=1
ifdef USE_REFL
CXXFLAGS+=-DUSE_REFL
endif

CXXFLAGS+=-I.

all: SDL_CHECK $(TARGET)

include sdl.mak
ifeq ($(SDL_VER),1)
CXXFLAGS+=-DSDL1
else
CXXFLAGS+=-DSDL2
endif

CXXFLAGS+=$(SDL_FLAGS)
LDLIBS+=$(SDL_LIBS)

realist: realist.cpp vec.h CSDL.h

clean:
	$(RM) $(TARGET)

clobber: clean
	$(RM) *~
