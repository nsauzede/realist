SRC:=$(wildcard main*.rs)
PPM:=$(patsubst %.rs,%.ppm,$(SRC))

RSC=rustc

all: $(PPM)
	md5sum *.ppm

OPT=-O
#OPT=-O0 -g
#DEBUG:=1
ifdef DEBUG
OPT+=-DDEBUG
endif

#PROF:=1
ifdef PROF
OPT+=-pg
endif

%.elf: %.rs pcg.rs
	$(RSC) -o $@ $< $(OPT) -lm

%.ppm: %.elf
	./$^ $(ARGS) > $@

clean:
	$(RM) *.elf *.ppm
