TARGET:=
# TARGET+=main02_2.ppm
TARGET+=main
TARGET+=smain
TARGET+=smain.html
TARGET+=s2main
TARGET+=s2main.html
# TARGET+=main02_3.ppm
# TARGET+=main03_3.ppm

ifdef DBG
CFLAGS:=-g -O0
else
#CFLAGS:=-O2
CFLAGS:=-O3 -fno-plt -flto -DNDEBUG
endif
LDLIBS:=-lm

SDL_CFLAGS+=`sdl-config --cflags`
SDL_LDLIBS+=`sdl-config --libs`
SDL2_CFLAGS+=`sdl2-config --cflags`
SDL2_LDLIBS+=`sdl2-config --libs`

all:$(TARGET)
MAINS:=$(wildcard main*_*.c)
main: main.c $(MAINS)
smain:CFLAGS+=$(SDL_CFLAGS)
smain:LDLIBS+=$(SDL_LDLIBS)
s2main:CFLAGS+=$(SDL2_CFLAGS)
s2main:LDLIBS+=$(SDL2_LDLIBS)
smain: smain.c $(MAINS)
s2main: s2main.c $(MAINS)
s2main.html:EMFLAGS=-s USE_SDL=2

SRC:=$(wildcard main*_*.c)
#EXE:=$(patsubst %.c,%.elf,$(SRC))
PPM:=$(patsubst %.c,%.ppm,$(SRC))

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

%.ppm: main
	./main $(*F) > $@

%.html: %.c $(MAINS)
	emcc $(EMFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET) *.ppm *.html *.js *.wasm
