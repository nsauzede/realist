TARGET=realist
TARGET+=raycpp
TARGET+=rayc

GO=go
ifeq (x$(shell which $(GO) > /dev/null ; echo $$?),x0)
HAVE_GO=1
endif

ifdef HAVE_GO
TARGET+=raygo.exe
endif

CFLAGS=-Wall -Werror
CFLAGS+=-Wextra

CXXFLAGS=-Wall -Werror
CXXFLAGS+=-Wextra

CFLAGS+=-g

CXXFLAGS+=-g

CFLAGS+=-pipe
CXXFLAGS+=-pipe

LDFLAGS+=-pipe

CXXFLAGS+=-std=c++11

CFLAGS+=-std=c99

LDLIBS+=-lm

USE_OPT=1
ifdef USE_OPT
CXXFLAGS+=-O3
CFLAGS+=-O3
CXXFLAGS+=-DUSE_OPT
else
CXXFLAGS+=-O0
CFLAGS+=-O0
endif

USE_VEC=1
ifdef USE_VEC
CXXFLAGS+=-DUSE_VEC
endif

#USE_FLASH=1
ifdef USE_FLASH
CXXFLAGS+=-DUSE_FLASH
endif

#USE_REFL=1
ifdef USE_REFL
CXXFLAGS+=-DUSE_REFL
endif

#USE_LAMP=1
ifdef USE_LAMP
CXXFLAGS+=-DUSE_LAMP
endif

ifdef STATIC
LDFLAGS+=-static
endif

CXXFLAGS+=-I.

all: SDL_CHECK $(TARGET)

include sdl.mak
ifeq ($(SDL_VER),1)
SDL_CXXFLAGS+=-DSDL1
else
SDL_CXXFLAGS+=-DSDL2
endif

SDL_CXXFLAGS+=$(SDL_FLAGS)
SDL_LDLIBS+=$(SDL_LIBS)

realist: CXXFLAGS+=$(SDL_CXXFLAGS)
realist: LDLIBS+=$(SDL_LDLIBS)

raygo.exe:
	GOPATH=$(shell pwd)/go $(GO) build go/raygo.go

realist: realist.cpp vec.h CSDL.h

clean:
	$(RM) $(TARGET)

clobber: clean
	$(RM) *~
