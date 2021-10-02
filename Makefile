TARGET=realist
TARGET+=raycpp
TARGET+=rayc

GO=go
ifeq (x$(shell which $(GO) > /dev/null ; echo $$?),x0)
HAVE_GO=1
endif

HAVE_V:=1
ifdef HAVE_V
TARGET+=rayv
endif

ifdef HAVE_GO
TARGET+=raygo
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

CFLAGS+=-std=gnu99

LDLIBS+=-lm

USE_OPT=1
ifdef USE_OPT
CXXFLAGS+=-O3 -flto
CFLAGS+=-O3 -flto
#CXXFLAGS+=-DUSE_OPT
else
CXXFLAGS+=-O0
CFLAGS+=-O0
endif

#USE_VEC=1
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

raygo:
#	GOPATH=$(shell pwd)/ray_go $(GO) build -o $@ ray_go/raygo.go
	GOPATH=`pwd`/ray_go $(GO) build -o $@ ray_go/raygo.go

V:=v/v
v/v:
	git clone https://github.com/vlang/v
	(cd $(@D) ; $(MAKE) ; cd -)

#VFLAGS:=-debug -show_c_cmd
rayv_v.c: $(V)
	cd ray_v ; ../$(V) $(VFLAGS) -o ../$@ . ; cd ..

rayv: rayv_v.c
	$(CC) -w $(CFLAGS)  $(LDFLAGS) $^ -lm -o $@

realist: realist.cpp vec.h CSDL.h

BENCH_SIZE:=10000
BENCH_ARGS=$(BENCH_SIZE) $(BENCH_SIZE)
BENCH_TARGETS:= rayc raycpp rayv
ifdef HAVE_GO
BENCH_TARGETS+=raygo
endif
bench: $(BENCH_TARGETS)
	/usr/bin/time ./rayc $(BENCH_ARGS) rayc.ppm && md5sum rayc.ppm
	/usr/bin/time ./rayv $(BENCH_ARGS) rayv.ppm && md5sum rayv.ppm
	/usr/bin/time ./raycpp $(BENCH_ARGS) raycpp.ppm && md5sum raycpp.ppm
ifdef HAVE_GO
	/usr/bin/time ./raygo $(BENCH_ARGS) raygo.ppm && md5sum raygo.ppm
endif

benchpy: bench
	/usr/bin/time ./raypy.py $(BENCH_ARGS) > raypy.ppm && md5sum raypy.ppm

vgcheck: BENCH_SIZE=100
VGOPTS:=--leak-check=full
vgcheck: rayc rayv
	valgrind $(VGOPTS) ./rayc $(BENCH_ARGS) rayc.ppm
	valgrind $(VGOPTS) ./rayv $(BENCH_ARGS) rayv.ppm

clean:
	@$(RM) $(TARGET)

clobber: clean
	@$(RM) *~
	@$(RM) *_v.c
	@$(RM) *.ppm

mrproper: clobber
	@$(RM) -Rf v
	@$(MAKE) -C rtiow $@
