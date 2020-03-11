SRC:=$(wildcard A*.v)
PPM:=$(patsubst %.v,%.ppm,$(SRC))

V:=v

all: $(PPM)
	md5sum *.ppm

OPT:=-O3
#DEBUG:=1
ifdef DEBUG
VFLAGS:=--enable-globals -d dbg
else
VFLAGS:=--enable-globals
endif

#PROF:=1
ifdef PROF
OPT+=-pg
endif

%_v.c: %.v
	$(V) $(VFLAGS) -o $@ $^

%.elf: %_v.c
	$(CC) -o $@ $^ $(OPT) -lm

%.ppm: %.elf
	./$^ > $@

clean:
	$(RM) *_v.c *.elf *.ppm
