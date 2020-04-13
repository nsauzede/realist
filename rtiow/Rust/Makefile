SRC:=$(wildcard main*.rs)
PPM:=$(patsubst %.rs,%.ppm,$(SRC))

RSC=rustc

all: $(PPM)
	md5sum *.ppm

# 3 9s
#OPT=-C opt-level=3
# 2 7s
OPT=-C opt-level=2
# 1 43s
#OPT=-C opt-level=1
# 0 crash
#OPT=-C opt-level=0
#OPT=-O0 -g
#DEBUG:=1
ifdef DEBUG
OPT+=--cfg 'DEBUG'
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
