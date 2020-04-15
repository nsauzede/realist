SRC:=$(wildcard main*.v)
EXE:=$(patsubst %.v,%.elf,$(SRC))
PPM:=$(patsubst %.v,%.ppm,$(SRC))

V:=v

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

OPT:=-O3
#DEBUG:=1
ifdef DEBUG
#VFLAGS:=--enable-globals -d dbg
VFLAGS:=-d dbg
else
#VFLAGS:=--enable-globals
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
