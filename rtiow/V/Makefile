SRC:=$(wildcard main*.v)
EXE:=$(patsubst %.v,%.elf,$(SRC))
PPM:=$(patsubst %.v,%.ppm,$(SRC))

V:=v

all: $(EXE)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

bench: main14.elf
	time ./main14.elf 1024 768 10 main14.ppm && md5sum main14.ppm

#DEBUG:=1
VFLAGS+=--enable-globals
#VFLAGS+=-profile profile.txt
ifdef DEBUG
VFLAGS+=-d dbg
else
# OPT:=-O3
# OPT:=-Ofast -fno-plt -flto -DNDEBUG
OPT:=-O3 -fno-plt -flto -DNDEBUG
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
