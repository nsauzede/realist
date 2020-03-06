SRC:=$(wildcard A*.v)
PPM:=$(patsubst %.v,%.ppm,$(SRC))

V:=v

all: $(PPM)
	md5sum *.ppm

#DEBUG:=1
ifdef DEBUG
VFLAGS:=--enable-globals -d dbg
else
VFLAGS:=--enable-globals
endif
%_v.c: %.v
	$(V) $(VFLAGS) -o $@ $^

%.elf: %_v.c
	$(CC) -o $@ $^ -O3 -lm

%.ppm: %.elf
	./$^ > $@

clean:
	$(RM) *_v.c *.elf *.ppm
