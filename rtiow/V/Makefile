SRC:=$(wildcard A*.v)
PPM:=$(patsubst %.v,%.ppm,$(SRC))

V:=v

all: $(PPM)
	md5sum *.ppm

%_v.c: %.v
	$(V) -o $@ $^

%.elf: %_v.c
	$(CC) -o $@ $^ -O3 -lm

%.ppm: %.elf
	./$^ > $@

clean:
	$(RM) *_v.c *.elf *.ppm
