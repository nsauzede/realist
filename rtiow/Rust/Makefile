SRC:=$(wildcard main*.rs)
PPM:=$(patsubst %.rs,%.ppm,$(SRC))

RSC=rustc

all: $(PPM)
	md5sum *.ppm

OPT=-C opt-level=3
#DEBUG:=1
ifdef DEBUG
OPT+=--cfg 'DEBUG'
endif

%.elf: %.rs pcg.rs
	$(RSC) -o $@ $< $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
