SRC:=$(wildcard main*.fs)
# EXE:=$(patsubst %.fs,%.elf,$(SRC))
PPM:=$(patsubst %.fs,%.ppm,$(SRC))

F:=gforth

all: $(PPM)
	du -sh .

check: $(PPM)
	md5sum *.ppm
	md5sum *.ppm | md5sum

bench:
	time $(F) ./main14.fs 1024 768 10 main14.ppm && md5sum main14.ppm

VFLAGS+=--enable-globals
#DEBUG:=1
ifdef DEBUG
VFLAGS+=-d dbg
#VFLAGS+=-profile profile.txt
else
# OPT:=-O3
# OPT:=-Ofast -fno-plt -flto -DNDEBUG
OPT:=-O3 -fno-plt -flto -DNDEBUG
endif

#PROF:=1
ifdef PROF
OPT+=-pg
endif

# %.elf: %_v.c
# 	$(CC) -o $@ $^ $(OPT) -lm

%.ppm: %.fs
	$(F) ./$^ > $@

clean:
	$(RM) *.elf *.ppm
